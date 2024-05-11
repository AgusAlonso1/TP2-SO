#include <linkedList.h>

typedef struct ProcessNode{
    ProcessADT processData;
    struct ProcessNode * next;
}ProcessNode;

typedef struct ProcessListCDT{
    ProcessNode * first;
    ProcessNode * last;
    uint64_t size;
}ProcessListCDT;

void add(ProcessListADT list, ProcessADT process) {
    ProcessNode * newProcess = allocMemory(sizeof(ProcessNode));
    newProcess->processData = process;
    newProcess->next = NULL;
    list->size++;
    if (list->first == NULL) {
        list->first = newProcess;
        list->last = newProcess;
        return;
    }
    list->last->next = newProcess;
    list->last = newProcess;
}

ProcessADT getFirstProcess(ProcessListADT list) {
    return list->first->processData;
}

ProcessADT popFirst(ProcessListADT list) {
    ProcessNode * popedNode = list->first;
    ProcessADT process = popedNode->processData;
    list->first = list->first->next;
    freeMemory(popedNode);
    list->size--;
    return process;
}



uint64_t getSize(ProcessListADT list) {
    return list->size;
}

uint64_t isEsmpty(ProcessListADT list) {
    return list->size > 0;
}
 


