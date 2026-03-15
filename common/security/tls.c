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

    // setting the algorithm key to be found on the tls to avoid "no suitable signature algorithm"
    if (SSL_CTX_set1_sigalgs_list(ctx, "ed25519:ecdsa_secp256r1_sha256:rsa_pss_rsae_sha256") != 1) {
        fprintf(stderr, "Erro ao configurar Signature Algorithms\n");
    }

    // setting group to key exchange X25519 (ideal to work with ed25519 (signature))
    if (SSL_CTX_set1_groups_list(ctx, "X25519:P-256:P-384") != 1) {
        fprintf(stderr, "Erro ao configurar Groups\n");
    }

    // loading certificate and key into tls
    if (SSL_CTX_use_certificate_file(ctx, "common/security/certificates/cert.pem", SSL_FILETYPE_PEM) <= 0)
    {
        printf("Certificate not found\n");
        ERR_print_errors_fp(stderr);    // real OpenSSL error
        return 1;
    }

    // setting the private key to the tls
    if (SSL_CTX_use_PrivateKey_file(ctx, "common/security/keys/key.pem", SSL_FILETYPE_PEM) <= 0)
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
    if (SSL_set_fd(ssl, client_fd) <= 0)
    {
        printf(stderr, "Error setting the file descriptor\n");
        ERR_print_errors(stderr);
        return 1;
    }

    // accept the connection to TLS
    int res = SSL_accept(ssl);
    if (res <= 0)
    {
        int err = SSL_get_error(ssl, res);
        fprintf(stderr, "Falha no SSL_accept. Código de erro SSL: %d\n", err);
        
        ERR_print_errors_fp(stderr);
        return 1;
    }

    return 0;
}