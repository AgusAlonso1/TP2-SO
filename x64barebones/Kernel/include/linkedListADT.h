#ifndef LINKED_LIST_ADT_H
#define LINKED_LIST_ADT_H

typedef struct LinkedListCDT * LinkedListADT;

typedef struct Node {
    void * data;
    struct Node * next;
} Node;

LinkedListADT createLinkedList();
void insert(LinkedListADT list, void *data);
int isEmpty(LinkedListADT list);
void removeNode(LinkedListADT list, Node * node);
Node * getFirst(LinkedListADT list);
int getListSize(LinkedListADT list);
void * removeFirst(LinkedListADT list); 

#endif