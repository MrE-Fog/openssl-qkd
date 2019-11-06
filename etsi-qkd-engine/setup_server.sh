#!/bin/bash

export OPENSSL_ENGINES="/usr/local/lib/engine"
export ENGINE_SOURCE="${HOME}/ripe-quantum-hackathon-nov-2019/etsi-qkd-engine"

if [[ ! -L ${OPENSSL_ENGINES}/etsi_qkd_server.dylib ]]; then
    ln -s ${ENGINE_SOURCE}/etsi_qkd_server.dylib ${OPENSSL_ENGINES}/etsi_qkd_server.dylib 
fi

if [[ ! -L ${OPENSSL_ENGINES}/etsi_qkd_client.dylib ]]; then
    ln -s ${ENGINE_SOURCE}/etsi_qkd_client.dylib ${OPENSSL_ENGINES}/etsi_qkd_client.dylib 
fi

export DYLD_FALLBACK_LIBRARY_PATH="${HOME}/openssl/:."
export OPENSSL_CONF="${ENGINE_SOURCE}/openssl_server.cnf"
