#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdint.h>

typedef struct SemaphoreListCDT * SemaphoreListADT;

void createSemaphoreManager(); 
int64_t semOpen(int64_t value, int64_t semId);
int64_t semWait(int64_t semId);
int64_t semPost(int64_t semId);
int8_t semClose(int64_t semId);
uint64_t getNewSemId();

#endif