/**
 * qkd_debug.h
 * 
 * Common code for debugging.
 * 
 * (c) 2019 Bruno Rijsman, All Rights Reserved.
 * See LICENSE for licensing information.
 */

#ifndef QKD_DEBUG_H
#define QKD_DEBUG_H

#include <stdbool.h>

void _QKD_fatal_if(const char *file, int line, const char *func, bool is_error, const char *msg);

#define QKD_fatal_if(is_error, msg) _QKD_fatal_if(__FILE__, __LINE__, __func__, is_error, msg)

void _QKD_fatal_with_errno_if(const char *file, int line, const char *func, bool is_error,
                              const char *msg);

#define QKD_fatal_with_errno_if(is_error, msg) _QKD_fatal_with_errno_if(__FILE__, __LINE__, \
                                                                        __func__, is_error, msg)

void _QKD_debug(const char *file, int line, const char *func, const char *format, ...);

#define QKD_debug(format, ...) _QKD_debug(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

#define QKD_enter(void) QKD_debug("Enter")

#define QKD_exit(void) QKD_debug("Exit")

#endif /* QKD_DEBUG_H */
