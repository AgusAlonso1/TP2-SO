#ifndef BUDDY_MEMORY_MANAGER_H
#define BUDDY_MEMORY_MANAGER_H

#include <stdint.h>
#include <definitions.h>
#include <memoryInfoADT.h>
#include <stdlib.h>

#define MAX_EXP 27 // 2^27 = 128 MB

#define FREE (uint8_t) 0
#define IN_USE (uint8_t) 1

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void * firstAdress, const uint64_t availableMem);
void * allocMemory(const uint64_t size);
void freeMemory(void * ptrToFree);

MemoryInfoADT getMemoryInfo();

//Despues borrar
MemoryManagerADT getMemoryManager();

// Despues mover de aca
uint64_t pow2(uint64_t argument);

#endif