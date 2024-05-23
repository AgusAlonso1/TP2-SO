#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <processes.h>

typedef struct ProcessListCDT * ProcessListADT; 

typedef struct ProcessNode{
    ProcessADT processData;
    uint8_t  quantumWating;
    struct ProcessNode * next;
    struct ProcessNode * prev;
}ProcessNode;

uint64_t getSize(ProcessListADT list);
void add(ProcessListADT list, ProcessADT process);
ProcessADT getFirstProcess(ProcessListADT list);
ProcessADT popFirst(ProcessListADT list);
void pop(ProcessListADT list, ProcessNode * node);
ProcessNode * getFirstNode(ProcessListADT list);
ProcessListADT newList();

#endif