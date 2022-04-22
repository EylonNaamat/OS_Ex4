//
// Created by eylon on 4/19/22.
//
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PORT_NUM 3424

int main(){
    int my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock == -1) {
        printf("socket creation failed...\n");
        exit(1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT_NUM);

    if (connect(my_sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(1);
    }
    printf("connected to the server..\n");

    char buf[2048];
    while(strcmp(buf, "EXIT")){
        printf("insert command\n");
        fgets(buf, 2048, stdin);
        size_t ln = strlen(buf)-1;
        if (buf[ln] == '\n') {
            buf[ln] = '\0';
        }
        if(send(my_sock, buf, 2048, 0) == -1){
            printf("sending to the server failed...\n");
            exit(1);
        }
        if(!strcmp(buf, "TOP") || !strcmp(buf, "POP")){
            if(recv(my_sock, buf, 2048, 0) == -1){
                printf("receiving from server failed...\n");
                exit(1);
            } 
            printf("%s\n", buf);
        }
    }

    close(my_sock);

    return 0;
}