#ifndef BUDDY_MEMORY_MANAGER_H
#define BUDDY_MEMORY_MANAGER_H

#include <stdint.h>
#include <kernelInfo.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const firstAdress, uint64_t const availableMem);
void *allocMemory(const uint64_t size);
void freeMemory(const void * ptrToFree);

#endif