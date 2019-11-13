/**
 * qkd_engine_common.c
 * 
 * Code that is common to both OpenSSL engines: the server engine (qkd_engine_server.c) and the client
 * engine (qkd_engine_client.c).
 * 
 * (c) 2019 Bruno Rijsman, All Rights Reserved.
 * See LICENSE for licensing information.
 */

#include "qkd_engine_common.h"
#include "qkd_debug.h"
#include <assert.h>
#include <string.h>
#include <openssl/engine.h>

bool QKD_return_fixed_key_for_testing = false; /* Use command line option orenvironment variable */
const unsigned long QKD_fixed_private_key = 1;
const unsigned long QKD_fixed_public_key = 2;

bool running_on_simulaqron = false;

/**
 * Convert an OpenSSL public key (which is stored as a big number) to an ETSI API key handle.
 * 
 * Returns 1 on success, 0 on failure (public key is too big for key handle).
 */
int QKD_bignum_to_key_handle(const BIGNUM *bn, QKD_key_handle_t *key_handle)
{
    int bn_num_bytes = BN_num_bytes(bn);
    if (bn_num_bytes > QKD_KEY_HANDLE_SIZE) {
        return 0;
    }
    int copied_bytes = BN_bn2bin(bn, (unsigned char *) key_handle->bytes);
    assert(copied_bytes == bn_num_bytes);
    int num_padding_bytes = QKD_KEY_HANDLE_SIZE - copied_bytes;
    memset((unsigned char *) key_handle->bytes + copied_bytes, 0, num_padding_bytes);
    return 1;
}

/**
 * Convert an ETSI API key handle to an OpenSSL public key (which is stored as a big number).
 */
void QKD_key_handle_to_bignum(const QKD_key_handle_t *key_handle, BIGNUM *bn)
{
    QKD_enter();
    BIGNUM *result_bn = BN_bin2bn((unsigned char *) key_handle->bytes, QKD_KEY_HANDLE_SIZE, bn);
    assert(result_bn == bn);
    QKD_return_success_void();
}

int QKD_shared_secret_nr_bytes(DH *dh)
{
    /* In real life the shared secret is a number between 1 and P-1, where P is the prime number
     * parameter of Diffie-Hellman. The shared secret is generated by asking QKD for a key using the
     * ETSI API. A realistic example is that we need 2048 bits = 256 bytes of shared secret.
     * However, generating 2048 bits of key material would take waaaay too long in simulation, so (
     * if we are running on top of SimulaQron we only ask for 8 bytes of shared secret. */
    if (running_on_simulaqron) {
        return 8;
    } else {
        const BIGNUM *p;
        DH_get0_pqg(dh, &p, NULL, NULL);
        return BN_num_bytes(p);
    }
}

/**
 * Bind this engine to OpenSSL, i.e. register all the engine functions.
 * 
 * Returns 1 on success, 0 on failure.
 */
int QKD_engine_bind(ENGINE *engine, const char *engine_id, const char *engine_name,
                    int (*generate_key) (DH *),
                    int (*compute_key) (unsigned char *key, const BIGNUM *pub_key, DH *dh),
                    ENGINE_GEN_INT_FUNC_PTR engine_init)
{
    QKD_enter();

    /* TODO: should we use init or app_data for anything? */
    int flags = 0;
    DH_METHOD *dh_method = DH_meth_new("ETSI QKD Client Method", flags);
    if (NULL == dh_method) {
        QKD_error("DH_new_method failed");
        QKD_return_error("%d", 0);
    }

    int result = DH_meth_set_generate_key(dh_method, generate_key);
    if (1 != result) {
        QKD_error("DH_meth_set_generate_key failed");
        QKD_return_error("%d", 0);
    }

    result = DH_meth_set_compute_key(dh_method, compute_key);
    if (1 != result) {
        QKD_error("DH_meth_set_compute_key failed");
        QKD_return_error("%d", 0);
    }

    result = ENGINE_set_id(engine, engine_id);
    if (1 != result) {
        QKD_error("ENGINE_set_id failed");
        QKD_return_error("%d", 0);
    }
    
    result = ENGINE_set_name(engine, engine_name);
    if (1 != result) {
        QKD_error("ENGINE_set_name failed");
        QKD_return_error("%d", 0);
    }

    result = ENGINE_set_DH(engine, dh_method);
    if (1 != result) {
        QKD_error("ENGINE_set_DH failed");
        QKD_return_error("%d", 0);
    }

    result = ENGINE_set_init_function(engine, engine_init);
    if (1 != result) {
        QKD_error("ENGINE_set_init_function failed");
        QKD_return_error("%d", 0);
    }

    QKD_return_success("%d", 1);
}
