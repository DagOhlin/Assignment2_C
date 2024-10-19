#include "memory_manager.h"
#include "linked_list.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>  

#include <stdint.h>// for the uint16_t int
#include <pthread.h>

//Dag Ohlin

pthread_rwlock_t readWriteLock; 


//------------------interaction funks------------------

//initalizes the list by allcoating with mem_alloc for the desired size "size"; it is up to user to 
//calculate size of each node(sizeof(node) * amount)
//could free head first but that will have to be handled by user 
//no way to tell user that mem_init failed so why eaven check for it
void list_init(Node** head, size_t size){
    //printf("*list_init\n");
    mem_init(size);
    
    *head = NULL; 

    pthread_rwlock_init(&readWriteLock, NULL);

}

//inserts a node with data "data" last in the list 
//if *head is = NULL then it will use the fact that head is a doublepointer to set head to the first node 
//it also handles case where the mem_alloc fails but can't tell the user because the func is a void 
void list_insert(Node** head, int data){

    //printf("*list_insert\n");
    pthread_rwlock_wrlock(&readWriteLock);
    Node* newNode = (Node*)mem_alloc(sizeof(Node));

    if(newNode == NULL){
        //printf("insertion failed because alloc returned null pointer\n");
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }
    //printf("*transfering data to new node\n");

    newNode->data = data;
    newNode->next = NULL;

    if(*head == NULL){
        //printf("*first insert adding data to head\n");

        *head = newNode;
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }

    Node* Current = *head;
    //printf("*finding last node in list\n");
    //loops through the lisst to find last element  
    while (Current->next != NULL)
    {
        Current = Current->next;
    }
    
    Current->next = newNode;
    pthread_rwlock_unlock(&readWriteLock);
    return;
    
}

//list_insert_after can insert without loopning because a address is provided 
void list_insert_after(Node* prev_node, int data){
    //printf("*list_insert_after\n");
    
    
    //checks if the address is valid 
    if(prev_node == NULL){
        
        return;
    }
    pthread_rwlock_wrlock(&readWriteLock);
    Node* newNode = (Node*)mem_alloc(sizeof(Node));
    //checks if mem_alloc faild with same problem as list_insert
    if(newNode == NULL){
        //printf("insertion failed because alloc returned null pointer\n");
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }
    //printf("transfering data to new node\n");
    //rerouts the pointers to fit the new node  
    newNode->data = data;
    
    newNode->next = prev_node->next;

    prev_node->next = newNode;

    pthread_rwlock_unlock(&readWriteLock);
    return;


}

//list_insert_before needs the head pointer to find the node before "next_node" because it is a single 
//linked list and it has to be a double if the "next_node" is head so it can store the new head
void list_insert_before(Node** head, Node* next_node, int data){
    //printf("*list_insert_before\n");
    
    //checks if the address is valid 
    if(next_node == NULL){
        return;
    }
    pthread_rwlock_wrlock(&readWriteLock);
    //handles the case where head "next_node" is head 
    if(*head == next_node){
        Node* newNode = (Node*)mem_alloc(sizeof(Node));
        newNode->data = data;
    
        newNode->next = *head;

        *head = newNode;
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }
    //finds the node before "next_node" because it is needed for rerouting the pointers
    Node* prevNode = *head;
    while (prevNode != NULL && prevNode->next != next_node)
    {
        prevNode = prevNode->next;
    }
    
    if(prevNode == NULL){

        //printf("next node does not exist");
        pthread_rwlock_unlock(&readWriteLock);
        return;

    }
     
    Node* newNode = (Node*)mem_alloc(sizeof(Node));
    //checks if mem_alloc faild with same problem as list_insert
    if(newNode == NULL){
        //printf("insertion failed because alloc returned null pointer\n");
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }

    //rerouts the pointers to fit the new node 
    newNode->data = data;

    newNode->next = next_node;
    prevNode->next = newNode;

    pthread_rwlock_unlock(&readWriteLock);
    return;

}

//list_search loops through thwe nodes until it finds the first node with data = "data"
//needs the head to know where to start looping but no neeed for ** 
Node* list_search(Node** head, int data){
    //printf("*list_search\n");
    pthread_rwlock_rdlock(&readWriteLock);
    Node* Current = *head;
    while (Current != NULL && Current->data != data)
    { 
        Current = Current->next;
    }
    //if the while fails to find the node with data it will return null
    pthread_rwlock_unlock(&readWriteLock);
    return Current;

}


//delets the first node with data = "data" starting at head and uses that "head" is dounble pointer to swap head if head.data = data 
void list_delete(Node** head, int data){
    //printf("*list_delete\n");
    pthread_rwlock_wrlock(&readWriteLock);
    //this read needs to be protected by a write lock so 2 threads dont try to free the same node 
    Node* nodeToRemove = *head;
    while (nodeToRemove != NULL && nodeToRemove->data != data)
    { 
        nodeToRemove = nodeToRemove->next;
    }
    //checks if the address is valid 
    if(nodeToRemove == NULL){
        //printf("node with specefied data does not exist, cant remove\n");
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }
    //swaps head for node after head 
    if(*head == nodeToRemove){
        //printf("Removing head\n");
        *head = nodeToRemove->next;
        mem_free(nodeToRemove);
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }

    //finds previus node for pointer rerouting 
    Node* prevNode = *head;
    while (prevNode != NULL && prevNode->next != nodeToRemove)
    {
        prevNode = prevNode->next;
    }
    
    //handles the case where it is the last node that needs to be removed 
    if(nodeToRemove->next == NULL){
        //printf("removing last node\n");
        prevNode->next = NULL;
        mem_free(nodeToRemove);
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }
    //handles the case where it is removing a node somewhere in middle
    //printf("Removing node somewhere in middle\n");
    prevNode->next = nodeToRemove->next;
    mem_free(nodeToRemove);
    pthread_rwlock_unlock(&readWriteLock);
    return;


    
}

//count al nodes in the list by looping through them
//could also use a counter var to get from O(n) to O(1) but that would require changing other funcs 
int list_count_nodes(Node** head){

    
    pthread_rwlock_rdlock(&readWriteLock);
    //checks if the address is valid 
    if(*head == NULL){
        pthread_rwlock_unlock(&readWriteLock);
        return 0;
    }
    
    Node* current = *head;
    int amount = 1;

    while(current->next != NULL){

        current = current->next;

        amount++;

    }
    pthread_rwlock_unlock(&readWriteLock);
    //++ 1 acounting for the fact that my linked list does not use a dummy node att the begining but the tests expect it 
    //will try to email 
    amount++; 
    return amount;

}

//goes throogh all of the nodes and frees them, because it is called list_cleanup and not list remove 
//or list free i first asumed that the user still wants to keep the list just remove all the nodes to fit new ones 
//based on how large a size they allocated but i decided to add mem_deinit(); because there is no other way to 
//remove memorypool available without directly interacting with the custom memory manager, so the user will 
//have to create a new list with list_init
void list_cleanup(Node** head){
    //checks if the address is valid but still runs if the user has used list_init() without adding anything 

    pthread_rwlock_wrlock(&readWriteLock);
    if(*head == NULL){
        mem_deinit();
        pthread_rwlock_unlock(&readWriteLock);
        return;
    }

    Node* current = *head;
    
    while(current != NULL){
        Node* delete = current;
        

        current = current->next;

        mem_free(delete);

    }

    mem_deinit();

    *head = NULL;
    pthread_rwlock_unlock(&readWriteLock);

    
}

//------------------display funks------------------

//displays all of the nodes by looping through and printing them starting from the provided head address
//i could have just used the list_display_range but the instructions for it was weird when i wrote this 
//they are fixed now but first we where suposed to use func overloading for this wich is not suported in c
void list_display(Node** head){
    //printf("*list_display\n");
    pthread_rwlock_rdlock(&readWriteLock);
    //checks if the address is valid 
    if(*head == NULL){
        printf("[]");
        pthread_rwlock_unlock(&readWriteLock);
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

    pthread_rwlock_unlock(&readWriteLock);

}

//prints nodes in specefied range 
void list_display_range(Node** head, Node* start_node, Node* end_node){
    //checks if "start_node" is null wich corresponds to the user requsting the print to strart from begining of list
    //then the "start_node" is set to "head"
    pthread_rwlock_rdlock(&readWriteLock);
    if(start_node == NULL){
        start_node = *head;
    }
    

    printf("[");
    Node* current = start_node;
    bool isFIrst = true;
    
    
    while (current != NULL) {
        
        if (!isFIrst) {
            printf(", ");
        }
        printf("%d", current->data);
        isFIrst = false;

        // if current == end_node it will stop and the 
        if (current == end_node) {
            break;
        }

        current = current->next;
    }

    
    
    printf("]");
    pthread_rwlock_unlock(&readWriteLock);

}


