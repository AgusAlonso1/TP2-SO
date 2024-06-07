#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdint.h>

#define SEMAPHORE_MANAGER_ADDRESS 

typedef struct SemaphoreListCDT * SemaphoreListADT;

void createSemaphoreManager(); 
uint64_t semOpen(uint64_t value);
uint64_t semWait(uint64_t semId);
uint64_t semPost(uint64_t semId);
uint8_t semClose(uint64_t semId);

#endif