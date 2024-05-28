#include <linkedList.h>
#include <stddef.h>

typedef struct ProcessListCDT{
    ProcessNode * first;
    ProcessNode * last;
    uint64_t size;
}ProcessListCDT;

void add(ProcessListADT list, ProcessADT process) {
    ProcessNode * newProcess = allocMemory(sizeof(ProcessNode));
    newProcess->processData = process;
    newProcess->next = NULL;
    newProcess->prev = list->last;
    newProcess->quantumWating = 0;
    list->size++;
    if (list->first == NULL) {
        list->first = newProcess;
        list->last = newProcess;
        return;
    }
    list->last->next = newProcess;
    list->last = newProcess;
}

ProcessListADT newList(){
    ProcessListADT list = allocMemory(sizeof(ProcessListCDT));
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    return list;
}

ProcessADT getFirstProcess(ProcessListADT list) {
    return list->first->processData;
}

ProcessNode * getFirstNode(ProcessListADT list) {
    return list->first;
}

ProcessADT popFirst(ProcessListADT list) {
    ProcessNode * popedNode = list->first;
    ProcessADT process = popedNode->processData;
    list->first = list->first->next;
    freeMemory(popedNode);
    list->size--;
    return process;
}

void pop(ProcessListADT list, ProcessNode * node) {
    if(node == NULL) {
        return;
    }

    if(node == list->first) {
        list->first = node->next;
        if(list->first != NULL) {
            list->first->prev = NULL;
        }
    }
    if(node == list->last) {
        list->last = node->prev;
        if(list->last != NULL) {
            list->last->next = NULL;
        }
    }
    if(node->prev!=NULL) {
        node->prev->next = node->next;
    }
    if(node->next != NULL) {
        node->next->prev = node->prev;
    }
    freeMemory(node);
}



uint64_t getSize(ProcessListADT list) {
    return list->size;
}

uint64_t isEsmpty(ProcessListADT list) {
    return list->size > 0;
}
 


