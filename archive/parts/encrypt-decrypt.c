#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int main (void)
{


    unsigned char key[32];
    unsigned char iv[16];
    RAND_priv_bytes(key, 32);
    RAND_priv_bytes(iv, 16);

    // encrypt the content of alice.txt and write it to alice.enc with fread

    char *filepath = "/home/kali/Documents/Developpement/Lab5/alice.txt";
    // in a new variable, dynamically append .enc to the filepath
    char *filepath_enc = malloc(strlen(filepath) + 5);
    strcpy(filepath_enc, filepath);
    strcat(filepath_enc, ".enc");


    FILE *toEncrypt = fopen(filepath, "r");
    FILE *fileEncrypted = fopen(filepath_enc, "w");
    unsigned char *plaintext = malloc(1024);
    unsigned char *ciphertext = malloc(1040);
    int plaintext_len = 1024;
    while(plaintext_len == 1024){
        plaintext_len = fread(plaintext, 1, 1024, toEncrypt);
        int ciphertext_len = encrypt(plaintext, plaintext_len, key, iv, ciphertext);
        fwrite(ciphertext, 1, ciphertext_len, fileEncrypted);
        
    }
    fclose(toEncrypt);
    fclose(fileEncrypted);
    free(plaintext);
    free(ciphertext);

    // decrypt the content of alice.enc and write it to alice.dec with fwrite

    FILE *toDecrypt = fopen(filepath_enc, "r");
    FILE *fileDecrypted = fopen("alice.dec", "w");
    unsigned char *decryptedtext = malloc(1024);
    unsigned char *ciphertext2 = malloc(1040);
    int decryptedtext_len = 0;
    int ciphertext_len2 = 1040;
    while(ciphertext_len2 == 1040){
        ciphertext_len2 = fread(ciphertext2, 1, 1040, toDecrypt);
        decryptedtext_len = decrypt(ciphertext2, ciphertext_len2, key, iv, decryptedtext);
        fwrite(decryptedtext, 1, decryptedtext_len, fileDecrypted);
        
        
    }
    fclose(toDecrypt);
    fclose(fileDecrypted);
    free(decryptedtext);
    free(ciphertext2);

}
