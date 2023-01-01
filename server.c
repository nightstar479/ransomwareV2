#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#define BUFFER_SIZE 1024

void handle_error(){
        perror("Connection error");
        exit(1);
}

int main(){
    // create a socket
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    int server_port= 9999;
    char *server_ip = "127.0.0.1";

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
        if(n < 0){
            perror("Listen error");
            handle_error();
        }

        len = sizeof(client_addr);
        client_socket = accept(sockid, (struct sockaddr*)&client_addr, &len);

        if(client_socket < 0){
            perror("Error during accept()\n");
            handle_error();
        }

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
        

        close(sockid);
    }
    return 0;
}