//
// Created by eylon on 4/22/22.
//
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>

#define PORT_NUM 3490

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

    char buf[2048] = "PUSH netzer";

    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    strcpy(buf, "TOP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

//    printf("%s", buf);
    assert(!strcmp(buf, "OUTPUT: netzer"));

    strcpy(buf, "POP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }
    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

    strcpy(buf, "TOP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

//    printf("%s", buf);
    assert(!strcmp(buf, "ERROR: stack is empty"));

    strcpy(buf, "PUSH mordechai");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }
    strcpy(buf, "PUSH amit");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }
    strcpy(buf, "PUSH dvir");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    strcpy(buf, "TOP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

//    printf("%s", buf);
    assert(!strcmp(buf, "OUTPUT: dvir"));

    strcpy(buf, "POP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }
    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

    strcpy(buf, "TOP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

//    printf("%s", buf);
    assert(!strcmp(buf, "OUTPUT: amit"));

    strcpy(buf, "POP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }
    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

    strcpy(buf, "TOP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

//    printf("%s", buf);
    assert(!strcmp(buf, "OUTPUT: mordechai"));

    strcpy(buf, "POP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }
    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

    strcpy(buf, "TOP");
    if(send(my_sock, buf, 2048, 0) == -1){
        printf("sending to the server failed...\n");
        exit(1);
    }

    if(recv(my_sock, buf, 2048, 0) == -1){
        printf("receiving from server failed...\n");
        exit(1);
    }

//    printf("%s", buf);
    assert(!strcmp(buf, "ERROR: stack is empty"));

    printf("tests went successfully!!!!\n");

    return 0;
}

