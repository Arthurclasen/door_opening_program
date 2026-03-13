#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/provider.h>
#include <stdio.h>

int add_extensions(X509 *x509)
{
    X509_EXTENSION *ext = NULL;
    X509V3_CTX ctx;

    // initializes the extension context
    X509V3_set_ctx_nodb(&ctx);
    X509V3_set_ctx(&ctx, x509, x509, NULL, NULL, 0);

    // 1. adding key usage (signing and key cryptography)
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_key_usage, "critical, digitalSignature, keyEncipherment");
    if (unlikely(X509_add_ext(x509, ext, -1) != 1)) return 0;   // adding the extension
    X509_EXTENSION_free(ext);

    // 2. adding extended key usage (TLS server)
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_ext_key_usage, "serverAuth");
    if (unlikely(X509_add_ext(x509, ext, -1) != 1)) return 0;
    X509_EXTENSION_free(ext);

    // 3. adding subjective alternative name (SAN)
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_subject_alt_name, "DNS:authentication_server, DNS:localhost");
    if (unlikely(X509_add_ext(x509, ext, -1) != 1)) return 0;
    X509_EXTENSION_free(ext);

    return 1;
}

int main()
{
    int         a, b, c, d, e;              // exception handlers auxiliar
    const char  *cert_path, *key_path;      // certificate and key dir

    // load the modules to be used (default only)
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    OSSL_PROVIDER_load(libctx, "default");

    // key generator context
    // creating the key using ECDSA ED25519
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_from_name(libctx, "ED25519", NULL);
    
    // creating key on that pointer
    if (unlikely(EVP_PKEY_keygen_init(pctx) != 1))
    {
        printf("Error initializing key");
        return 1;
    }

    // the key itself
    EVP_PKEY *pkey = NULL;

    // generates the key calling the provider (context)
    if (unlikely(EVP_PKEY_keygen(pctx, &pkey) != 1))
    {
        printf("Error generating key");
        return 1;
    }

    // creation of the X509 certificate
    X509 *x509 = X509_new();

    // using X509 v3
    if (unlikely(X509_set_version(x509, 2) != 1))
    {
        printf("Error setting version");
        return 1;
    }

    /*
        CERTIFICATE ATTRIBUTES:
        - VALIDATION TIME
        - PUBLIC KEY
        - NAMING SECTION
        - KEY USAGE
    */

    // define the serial number as 1
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

    // 1. certification validation time
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31000000L);

    // 2. public key related to it certificate
    if (unlikely(X509_set_pubkey(x509, pkey) != 1))
    {
        printf("Error setting pkey\n");
        return 1;
    }

    // 3. naming section
    X509_NAME *name = X509_NAME_new();
    a = X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char *)"BR", -1, -1, 0);
    b = X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *)"Door Project", -1, -1, 0);
    c = X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"authentication_server", -1, -1, 0);
    
    d = X509_set_subject_name(x509, name);
    e = X509_set_issuer_name(x509, name);
    X509_NAME_free(name);   // free the name 
    if (unlikely(a != 1) || unlikely(b != 1) || unlikely(c != 1) || unlikely(d != 1) || unlikely(e != 1))
    {
        printf("Error during naming certificate\n");
        return 1;
    }

    // 4. adding more extensions (key usages) to the certificate
    if (unlikely(add_extensions(x509)) != 1)
    {
        printf("Error occured adding extensions\n");
        return 1;
    }

    // 5. sign with that public key (the algorithm is already defined in certificate)
    if (unlikely(X509_sign(x509, pkey, NULL) <= 0)) 
    {
        unsigned long err = ERR_get_error();
        char buf[256];
        ERR_error_string_n(err, buf, sizeof(buf));
        printf("Error signing: %s\n", buf);
        return 1;
    }

    // saves the certificate in a file
    cert_path = "./certificates/cert.pem";
    FILE *f = fopen(cert_path, "wb");
    PEM_write_X509(f, x509);
    fclose(f);

    // saves the private key in a file
    key_path = "./keys/key.pem";
    FILE *k = fopen(key_path, "wb");
    PEM_write_PrivateKey(k, pkey, NULL, NULL, 0, NULL, NULL);
    fclose(k);

    printf("X509 certificate using ED25519 generated!\n");

    // free the memory
    X509_free(x509);
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(pctx);
    OSSL_LIB_CTX_free(libctx);

    return 0;
}