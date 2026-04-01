#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifndef TLS_H
#define TLS_H

int tls_create(SSL_CTX *ctx, int security_level);

int tls_connect(SSL *ssl, int client_fd);

#endif