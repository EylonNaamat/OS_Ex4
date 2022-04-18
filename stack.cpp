//
// Created by eylon on 4/18/22.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Stack_node{
    char* data;
    struct Stack_node* next;

}Stack_node, *stack_node_point;

typedef struct Stack{
    stack_node_point head;
    int capacity;
} Stack, *stack_point;

stack_point init_stack(){
    stack_point new_stack = (stack_point)(malloc(sizeof(Stack)));
    if(new_stack){
        new_stack->head = NULL;
        new_stack->capacity = 0;
    }
    return new_stack;
}

bool push(stack_point curr_stack, char* data){
    stack_node_point new_elem = (stack_node_point)(malloc(sizeof(Stack_node)));
    if(new_elem){
        char* copy = (char*)(malloc(strlen(data) +1));
        strcpy(copy, data);
        new_elem->data = copy;
        new_elem->next = curr_stack->head;
        curr_stack->head = new_elem;
        (curr_stack->capacity)++;
        return true;
    }
    return false;
}

bool pop(stack_point curr_stack){
    if(curr_stack->capacity != 0){
        stack_node_point top = curr_stack->head;
        curr_stack->head = curr_stack->head->next;
        free(top->data);
        free(top);
        (curr_stack->capacity)--;
        return true;
    }
    return false;
}

char* top(stack_point curr_stack){
    if(curr_stack->capacity != 0){
        return curr_stack->head->data;
    }
    return NULL;
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


int main(){
    int cap = 0;
    char str[2048];
    bool flag = true;
//    printf("insert a capacity for the stack\n");
//    scanf("%d", &cap);
    stack_point stack = init_stack();
    while(flag){
        printf("insert command\n");
        fgets(str, 2048, stdin);
        size_t ln = strlen(str)-1;
        if (str[ln] == '\n') {
            str[ln] = '\0';
        }
        char* data = strtok(str, " ");

        if(!(strcmp(data, "PUSH"))){
            data = strtok(NULL, " ");
            push(stack, data);
        }else if(!(strcmp(data, "TOP"))){
            if(!top(stack)){
                printf("ERROR: stack is empty\n");
            }else{
                printf("OUTPUT: %s\n", top(stack));
            }
        }else if(!(strcmp(data, "POP"))){
            if(!pop(stack)){
                printf("ERROR: stack is empty\n");
            }
        }else if(!(strcmp(data, "EXIT"))){
            flag = false;
        }else{
            printf("ERROR: illegal command\n");
            return -1;
        }
    }
    return 0;
}





