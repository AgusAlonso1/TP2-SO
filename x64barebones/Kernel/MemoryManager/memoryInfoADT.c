#include <memoryInfoADT.h>

typedef struct MemoryInfoCDT {
    uint8_t * typeName;
    uint64_t memoryInUse;
    uint64_t memoryFree;
} MemoryInfoCDT;

void createMemoryInfo(MemoryInfoADT memoryInfo, uint64_t memSize) {
    #if defined BUDDY
        
    #elif defined OTHER

    #else

    #endif
    memoryInfo->memoryInUse = 0;
    memoryInfo->memoryFree = memSize;
}

void allocUpdateInfo(MemoryInfoADT memoryInfo, uint64_t block_size) {
    memoryInfo->memoryInUse += block_size;
    memoryInfo->memoryFree -= block_size;
}

void freeUpdateIndo(MemoryInfoADT memoryInfo, uint64_t block_size) {
    memoryInfo->memoryInUse -= block_size;
    memoryInfo->memoryFree += block_size;
}