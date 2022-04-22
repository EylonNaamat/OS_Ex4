//
// Created by eylon on 4/18/22.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

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






typedef struct Stack_node{
    char* data;
    struct Stack_node* next;

}Stack_node, *stack_node_point;

typedef struct Stack{
    stack_node_point head;
    int capacity;
} Stack, *stack_point;

stack_point init_stack(){
    stack_point new_stack = (stack_point)(my_malloc(sizeof(Stack)));
    if(new_stack){
        new_stack->head = NULL;
        new_stack->capacity = 0;
    }
    return new_stack;
}

bool push(stack_point curr_stack, char* data){
    stack_node_point new_elem = (stack_node_point)(my_malloc(sizeof(Stack_node)));
    if(new_elem){
        char* copy = (char*)(my_malloc(strlen(data) +1));
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
        my_free(top->data);
        my_free(top);
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
    my_free(curr_stack);
}


int main(){
    int cap = 0;
    char str[2048];
    bool flag = true;
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





