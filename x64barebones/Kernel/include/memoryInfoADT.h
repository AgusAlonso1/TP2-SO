#ifndef MEMORY_INFO_ADT
#define MEMORY_INFO_ADT

#include <stdint.h>
#include <globals.h>

typedef struct MemoryInfoCDT * MemoryInfoADT;

void createMemoryInfo(MemoryInfoADT memoryInfo, uint64_t memSize);
void allocUpdateInfo(MemoryInfoADT memoryInfo, uint64_t block_size);
void freeUpdateInfo(MemoryInfoADT memoryInfo, uint64_t block_size);

#endif