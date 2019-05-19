#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mymalloc.h"

// USE THIS GODDAMN MACRO OKAY
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8


typedef struct Node{
    int sizeOfData;
    int used; // free:0 used:1
    struct Node* prev;
    struct Node* next;
}Node;

Node* global_head;
Node* global_tail;

unsigned int round_up_size(unsigned int data_size) {
    if(data_size == 0)
        return 0;
    else if(data_size < MINIMUM_ALLOCATION)
        return MINIMUM_ALLOCATION;
    else
        return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

Node* search_first_fit(Node** last,unsigned int size){//修改这里
    Node* current = global_head;
    for(; current != NULL; current = current->next){
        if(current->sizeOfData >= size && current->used == 0){
            return current;
        }
    }
    return NULL;
}

Node* request_space(Node* last,unsigned int size){
    Node* block;
    block = sbrk(0);
    void* request = sbrk(size + sizeof(Node));
    if(request == (void*) -1){
        return NULL; // sbrk failed
    }
    
    if(last){
        last->next = block;
    }
    block->sizeOfData = size;
    block->next = NULL;
    block->prev = last;
    block->used = 1;
    global_tail = block;
    return block;
}


void list_print(Node* head){
    Node* current_node = head;
    while(current_node != NULL){
        printf("%d  ", current_node->sizeOfData);// for debugging, delete it later
        printf("used: %d",current_node->used);//for debugging, delete it later
        current_node = current_node->next;
        if(current_node != NULL){
            printf("->");
        }
    }
    printf("\n");
}

Node* split(Node* block,unsigned int size){
    Node* new_block = block;
    new_block = PTR_ADD_BYTES(new_block,sizeof(Node));
    new_block = PTR_ADD_BYTES(new_block,size);
    new_block->sizeOfData = block->sizeOfData - sizeof(Node)- size;
    block->sizeOfData = size;
    block->used = 1; 
    new_block->used = 0;
    new_block->prev = block;
    new_block->next = block->next;
    new_block->next->prev = new_block;
    block->next = new_block;
    return block;
}

Node* coalese(Node* block,int direction){// direction 0 means previous, 1 means next
    if(direction == 0){
        Node* new_block = block->prev;
        new_block->sizeOfData = new_block->sizeOfData+ block->sizeOfData + sizeof(Node);
        if(block->next == NULL){
            global_tail = block->prev;
            new_block->next = NULL;
            block = NULL;
        }else{
            new_block->next = block->next;
            block->next->prev = new_block;
            block = NULL;
        }
        return new_block;
    }else
    {
        Node* nextBlock = block->next;
        block->sizeOfData = block->sizeOfData + nextBlock->sizeOfData + sizeof(Node);
        if(block->next->next != NULL){
            block->next = block->next->next;
            nextBlock->next->prev = block;
        }else{
            block->next = NULL;
            global_tail = block;
        }
        return block;
    }
}


void* my_malloc(unsigned int size) {
    //int i = sizeof(Node);
    if(size == 0)
        return NULL;
    size = round_up_size(size);
    
    // ------- Don't remove anything above this line. -------
    // Here's where you'll put your code!
    Node* block;
    if(!global_head){//First call
        block = request_space(NULL, size);
        if(!block){
            return NULL;
        }
        global_head = block;
    }else{
        Node* last = global_tail;
        block = search_first_fit(&global_head, size);
        if(block==NULL){// Failed to find free block
            block = request_space(last, size);
            if(!block){
                return NULL;
            }
        }else{//Found free block
            //TODO: using splitting 不确定
            if((block->sizeOfData - size) >= sizeof(Node)){
                block = split(block, size);
            }
            block->used = 1;
        }
    }
    // AT LEAST SOME OF IT
    
    // USE FUNCTIONS
    
    // PLEASE

    //    PTR_ADD_BYTES(block,1);
    return (PTR_ADD_BYTES(block, sizeof(Node)));//return a pointer to the region after node
}

Node* get_block_ptr(void* ptr){
    
    return (PTR_ADD_BYTES(ptr, -sizeof(Node)));
}

void my_free(void* ptr) {
    if(ptr == NULL)
        return;
    // and here's where you free stuff.
    Node* block = get_block_ptr(ptr);
    block->used = 0;
    if((block->prev != NULL) && (block->prev->used == 0)){
        block = coalese(block, 0);
    }

    if((block->next != NULL) && (block->next->used == 0)){
        block = coalese(block, 1);
    }
    
    if(block == global_tail){
        if(block == global_head){
            global_head = NULL;
            global_tail = NULL;
        }else{
            global_tail = block->prev;
            global_tail->next = NULL;
            //   block->used = 0;
        }
        sbrk(0-sizeof(Node)-block->sizeOfData);
    }
}

