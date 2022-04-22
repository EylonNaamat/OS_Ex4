/*
** server.c -- a stream socket server demo
*/

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

#define PORT "3424"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold
////////////// free and maloc
typedef struct block_info {
    int free;
    size_t data_size;
    struct block_info* next_block;
} block_info;

static block_info* last_head = NULL;
void *my_malloc(size_t get_size)
{
    block_info* block= last_head;
    size_t size = get_size + sizeof(block_info);
    while (block!= NULL)
    {
        if(block->free != 0)
        {
            if(block->data_size>=get_size)
            {
                block->free =0;
                return ((void*)block) + sizeof(block_info);
            }
        }
        block = block->next_block;
    }

    block_info* new_block = (block_info*)sbrk(size);
    new_block->data_size=get_size;
    new_block->free = 0;
    new_block->next_block =last_head;
    last_head = new_block;
    return ((void*)new_block) +sizeof(block_info);
}

// void *my_calloc(size_t get_numbers,size_t get_one_size)
// {
//     size_t get_size = get_one_size*get_numbers;
//     block_info* block= last_head;
//     size_t size = get_size + sizeof(block_info);
//     while (block!= NULL)
//     {
//         if(block->free != 0)
//         {
//             if(block->data_size>=get_size)
//             {
//                 block->free =0;
//                 for(int i=0;i<get_size;i++)
//                 {
//                     *(((void*)block)+sizeof(block_info)+i) = 0;
//                 }
//                 return ((void*)block) + sizeof(block_info);
//             }
//         }
//         block = block->next_block;
//     }

//     block_info* new_block = (block_info*)sbrk(size);
//     new_block->data_size=get_size;
//     new_block->free = 0;
//     new_block->next_block =last_head;
//     last_head = new_block;
//     return ((void*)new_block) +sizeof(block_info);
// }

void my_free(void * my_point)
{
    block_info* my_block =(block_info*)(my_point - sizeof(block_info));
    my_block->free =1;
    int i = 0;
    if(my_point+((my_block->data_size))== sbrk(0))
    {
        while(my_block!=NULL)
        {
            i++;
            if(my_block->free == 0)
            {
                break;
            }
            else
            {
                block_info* tmp = my_block->next_block;
                sbrk(-(my_block->data_size+sizeof(block_info)));
                my_block = tmp;
            }
        }
        last_head = my_block;
    }
}

/////////end free and maloc


/////////// stack 


typedef struct Stack_node{
    char* data;
    struct Stack_node* next;

}Stack_node, *stack_node_point;

typedef struct Stack{
    stack_node_point head;
    int capacity;
} Stack, *stack_point;




// global variables
pthread_mutex_t mutex_push;
//pthread_mutex_t mutex_top;
//pthread_mutex_t mutex_pop;
stack_point stack;





stack_point init_stack(){
    stack_point new_stack = (stack_point)(my_malloc(sizeof(Stack)));
    if(new_stack){
        new_stack->head = NULL;
        new_stack->capacity = 0;
    }
    return new_stack;
}

void* push(char* data){
    pthread_mutex_lock(&mutex_push);
    stack_node_point new_elem = (stack_node_point)(my_malloc(sizeof(Stack_node)));
    if(new_elem){
        char* copy = (char*)(my_malloc(strlen(data) +1));
        strcpy(copy, data);
        new_elem->data = copy;
        new_elem->next = stack->head;
        stack->head = new_elem;
        (stack->capacity)++;
    }
    pthread_mutex_unlock(&mutex_push);
}

bool pop(){
    pthread_mutex_lock(&mutex_push);
    if(stack->capacity != 0){
        stack_node_point top = stack->head;
        stack->head = stack->head->next;
        my_free(top->data);
        my_free(top);
        (stack->capacity)--;
        pthread_mutex_unlock(&mutex_push);
        return true;
    }else{
        pthread_mutex_unlock(&mutex_push);
        return false;
    }
}

void* top(int* new_sock){
    pthread_mutex_lock(&mutex_push);
    if(stack->capacity != 0){
        char buf[2048] = "OUTPUT: ";
        int i;
        int j = 0;
        for(i = strlen(buf); (stack->head->data)[j] != '\0'; ++i, ++j){
            buf[i] = (stack->head->data)[j];
        }
        (stack->head->data)[i] = '\0';
        send((*new_sock), buf, 2048, 0);
    }else {
        char buf[2048] = "ERROR: stack is empty";
        send((*new_sock), buf, 2048, 0);
    }
    pthread_mutex_unlock(&mutex_push);
}

void clear(stack_point curr_stack){
    while(curr_stack->capacity){
        char ans[2048];
        pop();
    }
}

void destroy_stack(stack_point curr_stack){
    clear(curr_stack);
    my_free(curr_stack);
}



//////////end stack
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


void* sender(void* arg)
{
    int * new_fd = (int*)arg;
    char buf[2048];
    
    while(recv(*new_fd, buf, 2048, 0) != -1){
                size_t ln = strlen(buf)-1;
                if (buf[ln] == '\n') {
                    buf[ln] = '\0';
                }
                char command[5];
                int j;
                for(j = 0; buf[j] != ' ' && buf[j] != '\0'; ++j){
                    command[j] = buf[j];
                }
                command[j] = '\0';

                if(!(strcmp(command, "PUSH"))) {
                    char copy[2048];
                    int k;
                    j = j+1;
                    for(k = 0; buf[j] != '\0'; ++k, ++j){
                        copy[k] = buf[j];
                    }
                    copy[k] = '\0';
                    push(copy);

                }else if(!(strcmp(command, "TOP"))) {
                    top(new_fd);
                }else if(!(strcmp(command, "POP"))){
                    if(pop())
                    {
                        send((*new_fd), "OUTPUT: popping", 2048, 0);
                    }
                    else{
                        send((*new_fd), "ERROR: stack is empty", 2048, 0);
                    }
                }else if(!(strcmp(command, "EXIT"))){
                    break;
                }else{
                    printf("ERROR: illegal command\n");
                    break;
                }
    }
    close((*new_fd));

}


int main(void)
{
    int sockfd, new_fd[10];  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

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



    stack = init_stack();
    pthread_mutex_init(&mutex_push, NULL);
    //pthread_mutex_init(&mutex_top, NULL);
    //pthread_mutex_init(&mutex_pop, NULL);

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd[i] = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd[i] == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if(pthread_create(&thread_id[i], NULL, sender, (&new_fd[i])) != 0){
            printf("thread creation failed\n");
        }
        i++;
        if(i >= 5){
            for(int j=0;j<=i;j++)
            {
                pthread_join(thread_id[j], NULL);
            }
            i = 0;
        }
    }

    return 0;
}