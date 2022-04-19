//
// Created by eylon on 4/19/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define PORT "3509"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold


pthread_mutex_t mutex_push;
pthread_mutex_t mutex_top;
pthread_mutex_t mutex_pop;

typedef struct Stack_node{
    char* data;
    struct Stack_node* next;

}Stack_node, *stack_node_point;

typedef struct Stack{
    stack_node_point head;
    int capacity;
} Stack, *stack_point;

typedef struct push_arg{
    stack_point stack;
    char* data;
}push_arg, *push_arg_point;

stack_point init_stack(){
    stack_point new_stack = (stack_point)(malloc(sizeof(Stack)));
    if(new_stack){
        new_stack->head = NULL;
        new_stack->capacity = 0;
    }
    return new_stack;
}

void* push(void* arg){
    pthread_mutex_lock(&mutex_push);
    push_arg_point tmp = (push_arg_point)(arg);
    stack_node_point new_elem = (stack_node_point)(malloc(sizeof(Stack_node)));
    if(new_elem){
        char* copy = (char*)(malloc(strlen(tmp->data) +1));
        strcpy(copy, tmp->data);
        new_elem->data = copy;
        new_elem->next = tmp->stack->head;
        tmp->stack->head = new_elem;
        (tmp->stack->capacity)++;
    }
    pthread_mutex_unlock(&mutex_push);
}

void* pop(void* arg){
    pthread_mutex_lock(&mutex_pop);
    stack_point curr_stack = (stack_point)(arg);
    if(curr_stack->capacity != 0){
        stack_node_point top = curr_stack->head;
        curr_stack->head = curr_stack->head->next;
        free(top->data);
        free(top);
        (curr_stack->capacity)--;
    }else{
        printf("ERROR: stack is empty\n");
    }
    pthread_mutex_unlock(&mutex_pop);
}

void* top(void* arg){
    pthread_mutex_lock(&mutex_top);
    stack_point curr_stack = (stack_point)(arg);
    if(curr_stack->capacity != 0){
        printf("OUTPUT: %s\n", curr_stack->head->data);
    }else {
        printf("ERROR: stack is empty\n");
    }
    pthread_mutex_unlock(&mutex_top);
}

void clear(stack_point curr_stack){
    while(curr_stack->capacity){
        pop(curr_stack);
    }
}

void destroy_stack(stack_point curr_stack){
    clear(curr_stack);
    free(curr_stack);
}


void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void* sender(void* arg){
    int * new_fd = (int*)arg;
    if(send((*new_fd), "Hello, world!", 13, 0)== -1){
        close((*new_fd));
        exit(0);
    }
    close((*new_fd));
}


int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[2048];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    int i = 0;
    pthread_t thread_id[10];
    stack_point stack = init_stack();
    pthread_mutex_init(&mutex_push, NULL);
    pthread_mutex_init(&mutex_top, NULL);
    pthread_mutex_init(&mutex_pop, NULL);


    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if(recv(new_fd, buf, 2048, 0) != -1){
            if(i >= 5){
                i = 0;
            }
            size_t ln = strlen(buf)-1;
            if (buf[ln] == '\n') {
                buf[ln] = '\0';
            }
            char* data = strtok(buf, " ");
            if(!(strcmp(data, "PUSH"))) {
                data = strtok(NULL, " ");
                push_arg_point push_struct = (push_arg_point)(malloc(sizeof(push_arg)));
                push_struct->stack = stack;
                char* copy = (char*)(malloc(strlen(data) +1));
                strcpy(copy, data);
                push_struct->data = copy;

                if(pthread_create(&thread_id[i], NULL, push, (void*)(push_struct)) != 0){
                    printf("thread creation failed\n");
                }

                pthread_join(thread_id[i], NULL);
                i++;

            }else if(!(strcmp(data, "TOP"))) {
                if (pthread_create(&thread_id[i], NULL, top, (stack)) != 0) {
                    printf("thread creation failed\n");
                }
                pthread_join(thread_id[i], NULL);
                i++;

            }else if(!(strcmp(data, "POP"))){
                if(pthread_create(&thread_id[i], NULL, pop, (stack)) != 0){
                    printf("thread creation failed\n");
                }
                pthread_join(thread_id[i], NULL);
                i++;
            }else if(!(strcmp(data, "EXIT"))){
                break;
            }else{
                printf("ERROR: illegal command\n");
                return -1;
            }
        }
    }
    pthread_mutex_destroy(&mutex_push);
    pthread_mutex_destroy(&mutex_top);
    pthread_mutex_destroy(&mutex_pop);
    return 0;
}