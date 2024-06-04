#ifndef BUDDY_MEMORY_MANAGER_H
#define BUDDY_MEMORY_MANAGER_H

#include <stdint.h>
#include <definitions.h>

#define MAX_EXP 27 // 2^27 = 128 MB

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void * firstAdress, uint64_t const availableMem);
void *allocMemory(const uint64_t size);
void freeMemory(void * ptrToFree);
MemoryManagerADT getMemoryManager();

// Despues mover de aca
uint64_t pow2(uint64_t argument);

#endif