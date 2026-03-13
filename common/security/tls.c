#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int tls_create(SSL_CTX *ctx, int security_level)
{
    // initializing ssl library to TLS
    SSL_library_init();

    // loading possible errors
    SSL_load_error_strings();

    // setting the security level, recommended level 2 or 3 for ED25519
    SSL_CTX_set_security_level(ctx, security_level);

    // loading certificate and key into tls
    if (SSL_CTX_use_certificate_file(ctx, "common/certificates/cert.pem", SSL_FILETYPE_PEM) <= 0)
    {
        printf("Certificate not found\n");
        ERR_print_errors_fp(stderr);    // real OpenSSL error
        return 1;
    }

    // setting the private key to the tls
    if (SSL_CTX_use_PrivateKey_file(ctx, "common/keys/key.pem", SSL_FILETYPE_PEM) <= 0)
    {
        fprintf(stderr, "Error in private key\n");
        ERR_print_errors_fp(stderr);
        return 1;
    }

    return 0;
}

int tls_connect(SSL *ssl, int client_fd)
{
    // setting the client file descriptor to the tls connection
    if (SSL_set_fd(ssl, client_fd))
    {
        printf(stderr, "Error setting the file descriptor\n");
        ERR_print_errors(stderr);
        return 1;
    }

    // TLS handshake
    if (SSL_accept(ssl))
    {
        printf(stderr, "Error accepting connection\n");
        ERR_print_errors(stderr);
        return 1;
    }

    return 0;
}