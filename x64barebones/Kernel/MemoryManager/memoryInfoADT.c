// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryInfoADT.h>
#include <memoryManager.h>

void createMemoryInfo(MemoryInfo * memoryInfo, uint64_t memSize) {
    #if defined BUDDY
        my_strcopy(memoryInfo->typeName, "Buddy Allocator");
    #elif defined FL
        my_strcopy(memoryInfo->typeName, "Free-List Allocator");
    #endif
    memoryInfo->memoryInUse = 0;
    memoryInfo->memoryFree = memSize;
}

void allocUpdateInfo(MemoryInfo * memoryInfo, uint64_t block_size) {
    memoryInfo->memoryInUse += block_size;
    memoryInfo->memoryFree -= block_size;
}

void freeUpdateInfo(MemoryInfo * memoryInfo, uint64_t block_size) {
    memoryInfo->memoryInUse -= block_size;
    memoryInfo->memoryFree += block_size;
}

MemoryData * getMemoryInfoCopy(MemoryInfo * memoryInfo) {
    MemoryData * memoryDataCopy = allocMemory(sizeof(MemoryData));
    memoryDataCopy->typeName = (char *) allocMemory(my_strlen(memoryDataCopy->typeName) + 1);
    my_strcopy(memoryDataCopy->typeName, memoryInfo->typeName);
    memoryDataCopy->memoryFree = memoryInfo->memoryFree;
    memoryDataCopy->memoryInUse = memoryInfo->memoryInUse;

    return memoryDataCopy;
}