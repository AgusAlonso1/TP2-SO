#ifndef LINKED_LIST_ADT_H
#define LINKED_LIST_ADT_H

#include <stdint.h>
#include <processes.h>

typedef struct LinkedListCDT * LinkedListADT;

LinkedListADT createLinkedList();
void insert(LinkedListADT list, void *data);
void * remove(LinkedListADT list);
int isEmpty(LinkedListADT list);
void removeNode(LinkedListADT list, Node *node);

#end