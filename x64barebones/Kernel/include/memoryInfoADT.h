#ifndef MEMORY_INFO_ADT
#define MEMORY_INFO_ADT

#include <stdint.h>
#include <globals.h>
#include <libString.h>

#define TYPE_NAME_SIZE 20

typedef struct MemoryInfo {
    char typeName[TYPE_NAME_SIZE];
    uint64_t memoryInUse;
    uint64_t memoryFree;
} MemoryInfo;

void createMemoryInfo(MemoryInfo * memoryInfo, uint64_t memSize);
void allocUpdateInfo(MemoryInfo * memoryInfo, uint64_t block_size);
void freeUpdateInfo(MemoryInfo * memoryInfo, uint64_t block_size);
MemoryData * getMemoryInfoCopy(MemoryInfo * memoryInfo);

#endif