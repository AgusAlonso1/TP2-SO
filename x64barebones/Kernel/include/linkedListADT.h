#ifndef LINKED_LIST_ADT_H
#define LINKED_LIST_ADT_H

#include <stdint.h>
//#include <processes.h>

typedef struct LinkedListCDT * LinkedListADT;

typedef struct Node {
    void * data;
    struct Node * next;
} Node;

LinkedListADT createLinkedList();
void insert(LinkedListADT list, void *data);
//void * remove(LinkedListADT list);
int isEmpty(LinkedListADT list);
void removeNode(LinkedListADT list, Node * node);
Node * getFirst(LinkedListADT list);
uint64_t getListSize(LinkedListADT list);
void * removeFirst(LinkedListADT list); 

#endif