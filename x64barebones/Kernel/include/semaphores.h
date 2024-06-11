#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdint.h>

typedef struct SemaphoreListCDT * SemaphoreListADT;

void createSemaphoreManager(); 
int64_t semOpen(uint64_t value, uint64_t semId);
int64_t semWait(uint64_t semId);
int64_t semPost(uint64_t semId);
int8_t semClose(uint64_t semId);
uint64_t getNewSemId();

#endif