#include <memoryInfoADT.h>

#include <memoryManager.h>

#define TYPE_NAME_SIZE 20

typedef struct MemoryInfoCDT {
    uint8_t typeName[TYPE_NAME_SIZE];
    uint64_t memoryInUse;
    uint64_t memoryFree;
} MemoryInfoCDT;

void createMemoryInfo(MemoryInfoADT memoryInfo, uint64_t memSize) {
    #if defined BUDDY
        my_strcopy((char *) memoryInfo->typeName, "Buddy Allocator");
    #elif defined OTHER
        my_strcopy((char *) memoryInfo->typeName, "Free-List Allocator");
    #endif
    memoryInfo->memoryInUse = 0;
    memoryInfo->memoryFree = memSize;
}

void allocUpdateInfo(MemoryInfoADT memoryInfo, uint64_t block_size) {
    memoryInfo->memoryInUse += block_size;
    memoryInfo->memoryFree -= block_size;
}

void freeUpdateInfo(MemoryInfoADT memoryInfo, uint64_t block_size) {
    memoryInfo->memoryInUse -= block_size;
    memoryInfo->memoryFree += block_size;
}

MemoryData * getMemoryInfoCopy(MemoryInfoADT memoryInfo) {
    MemoryData * memoryDataCopy = allocMemory(sizeof(MemoryData));
    my_strcopy((char *) memoryDataCopy->typeName, (char *) memoryInfo->typeName);
    memoryDataCopy->memoryFree = memoryInfo->memoryFree;
    memoryDataCopy->memoryInUse = memoryInfo->memoryInUse;

    return memoryDataCopy;
}