// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <linkedListADT.h>
#include <processes.h>
#include <stddef.h>

typedef struct LinkedListCDT {
    Node * first;
    Node * last;
    uint64_t size;
} LinkedListCDT;


LinkedListADT createLinkedList() {
    LinkedListADT list = allocMemory(sizeof(LinkedListCDT));
    if (list == NULL) {
        return NULL;
    }
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    return list;
}

void insert(LinkedListADT list, void *data) {
    Node *newNode = allocMemory(sizeof(Node));
    if (newNode == NULL) {
        return;
    }
    newNode->data = data;
    newNode->next = NULL;
    if (list->last != NULL) {
        list->last->next = newNode;
    }
    list->last = newNode;
    if (list->first == NULL) {
        list->first = newNode;
    }
    list->size++;
}

void * removeFirst(LinkedListADT list) {
    if (list->first == NULL) {
        return NULL;
    }
    Node *temp = list->first;
    void *data = temp->data;
    list->first = list->first->next;
    if (list->first == NULL) {
        list->last = NULL;
    }
    freeMemory(temp);
    list->size--;
    return data;
}

int isEmpty(LinkedListADT list) {
    return list->first == NULL;
}

void removeNode(LinkedListADT list, Node *node) {
    if (list == NULL || node == NULL) {
        return;
    }

    if (list->first == node) {
        list->first = node->next;
        if (list->first == NULL) {
            list->last = NULL;
        }
    } else {
        Node *current = list->first;
        while (current != NULL && current->next != node) {
            current = current->next;
        }
        if (current != NULL) {
            current->next = node->next;
            if (current->next == NULL) {
                list->last = current;
            }
        }
    }
    freeMemory(node);
    list->size--;
}

Node * getFirst(LinkedListADT list) {
    return list->first;
}

int getListSize(LinkedListADT list) {
    return list->size;
}