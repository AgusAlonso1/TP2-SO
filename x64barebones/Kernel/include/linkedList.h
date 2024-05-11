#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <processes.h>
//#include <memoryManager.h>

typedef struct ProcessListCDT * ProcessListADT; 

uint64_t getSize(ProcessListADT list);
void add(ProcessListADT list, ProcessADT process);
ProcessADT getFirstProcess(ProcessListADT list);
ProcessADT popFirst(ProcessListADT list);

#endif