#include "memory_manager.h"
#include "linked_list.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_defs.h"
#include <stdint.h>
//yeah might be nessesary to add a bunch of null checks for the params but there is now way to comunicate that to user anyways so it will make no differece
/*
typedef struct{
    uint16_t data; // Stores the data as an unsigned 16-bit integer
    struct Node* next; // A pointer to the next node in the List
} Node;
*/
//------------------interaction funks------------------
void list_init(Node** head){
    printf("*list_init\n");
    *head = NULL; 

}

void list_insert(Node** head, int data){

    printf("*list_insert\n");

    Node* newNode = (Node*)mem_alloc(sizeof(Node));

    if(newNode == NULL){
        printf("insertion failed because alloc returned null pointer\n");
        return;
    }
    printf("*transfering data to new node\n");

    newNode->data = data;
    newNode->next = NULL;

    if(*head == NULL){
        printf("*first insert adding data to head\n");

        *head = newNode;

        return;
    }

    Node* Current = *head;
    printf("*finding last node in list\n");
    while (Current->next != NULL)
    {
        Current = Current->next;
    }
    
    Current->next = newNode;

    return;
    
}


void list_insert_after(Node* prev_node, int data){
    printf("*list_insert_after\n");
    //do i need to check that the node pointer is to an actual node?


    if(prev_node == NULL){
        return;
    }

    Node* newNode = (Node*)mem_alloc(sizeof(Node));

    if(newNode == NULL){
        printf("insertion failed because alloc returned null pointer\n");
        return;
    }
    printf("transfering data to new node\n");

    newNode->data = data;
    
    newNode->next = prev_node->next;

    prev_node->next = newNode;

    return;


}


void list_insert_before(Node** head, Node* next_node, int data){
    printf("*list_insert_before\n");
    

    if(next_node == NULL){
        return;
    }

    if(*head == next_node){
        Node* newNode = (Node*)mem_alloc(sizeof(Node));
        newNode->data = data;
    
        newNode->next = *head;

        *head = newNode;
        return;
    }
    Node* prevNode = *head;
    while (prevNode != NULL && prevNode->next != next_node)
    {
        prevNode = prevNode->next;
    }
    
    if(prevNode == NULL){

        printf("next node does not exist");
        return;

    }

    Node* newNode = (Node*)mem_alloc(sizeof(Node));

    newNode->data = data;

    newNode->next = next_node;
    prevNode->next = newNode;



}

Node* list_search(Node** head, int data){
    printf("*list_search\n");
    Node* Current = *head;
    while (Current != NULL && Current->data != data)
    { 
        Current = Current->next;
    }
    
    return Current;

}

void list_delete(Node** head, int data){
    printf("*list_delete\n");
    Node* nodeToRemove = list_search(head, data); 

    if(nodeToRemove == NULL){
        printf("node with specefied data does not exist, cant remove\n");
        return;
    }

    if(*head == nodeToRemove){
        printf("Removing head\n");
        *head = nodeToRemove->next;
        mem_free(nodeToRemove);
        return;
    }


    Node* prevNode = *head;
    while (prevNode != NULL && prevNode->next != nodeToRemove)
    {
        prevNode = prevNode->next;
    }
    

    if(nodeToRemove->next == NULL){
        printf("removing last node\n");
        prevNode->next = NULL;
        mem_free(nodeToRemove);
        return;
    }
    printf("Removing node somewhere in middle\n");
    prevNode->next = nodeToRemove->next;
    mem_free(nodeToRemove);
    return;


    
}

int list_count_nodes(Node** head){

    if(*head == NULL){
        return 0;
    }

    Node* current = *head;
    int amount = 1;

    while(current->next != NULL){

        current = current->next;

        amount++;

    }

    return amount;

}

void list_cleanup(Node** head){

    if(*head == NULL){
        return;
    }

    Node* current = *head;
    
    while(current != NULL){
        Node* delete = current;
        

        current = current->next;

        mem_free(delete);

    }

    *head = NULL;
}

//------------------display funks------------------

void list_display(Node** head){
    printf("*list_display\n");
    if(*head == NULL){
        printf("[]\n");
        return;
    }

    Node* Current = *head;
     printf("[");
    while (Current != NULL)
    { 
        printf("%d", Current->data);
        Current = Current->next;
        if(Current != NULL){
            printf(", ");
        }
    }

    printf("]\n");



}


void list_display_range(Node** head, Node* start_node, Node* end_node){

    if(start_node == 0){
        start_node = *head;
    }
    if(end_node == -1){
        end_node = NULL;
    }

    printf("[");
    Node* current = start_node;
    bool isFIrst = true;
    bool startPrinting = false;

     while (current != NULL) {
        
        if (!isFIrst) {
            printf(", ");
        }
        printf("%d", current->data);
        isFIrst = false;

        
        if (current == end_node) {
            break;
        }

        current = current->next;
    }

    
    
    printf("]");

}


/*
int main(){

    // i am initing the mempool in here for now cuz why would that be done in the linked list how would it know how long you want it the pool has no expand funk
    mem_init(sizeof(Node) * 70);
    Node *head = NULL;

    list_init(&head);

    list_insert(&head, 45);
    list_insert(&head, 7);
    list_insert(&head, 3456);
    list_insert(&head, 2);

    list_display(&head);

    list_insert_before(&head ,list_search(&head, 7), 56);
    list_insert_before(&head ,head, 9);

    list_display(&head);
    

    list_delete(&head, 9);

    list_display(&head);

    

    


    getchar();
    return 0;
}
*/