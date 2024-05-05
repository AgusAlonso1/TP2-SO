#ifndef MEMORY_INFO_ADT
#define MEMORY_INFO_ADT

#include <stdint.h>

typedef struct MemoryInfoCDT * MemoryInfoADT;

void createMemoryInfo(MemoryInfoADT memoryInfo, uint64_t memSize);
void allocUpdateInfo(MemoryInfoADT memoryInfo, uint64_t block_size);
void freeUpdateIndo(MemoryInfoADT memoryInfo, uint64_t block_size);

#endif