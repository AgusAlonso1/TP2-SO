#ifdef FL
#include <memoryManager.h>
#include <stdlib.h>

#define HEADER_FOOTER_SIZE (sizeof(MemoryChunkHeader) + sizeof(MemoryChunkFooter))
#define HEADER_SIZE sizeof(MemoryChunkHeader)
#define FOOTER_SIZE sizeof(MemoryChunkFooter)

#define NO_COAELESCING 0
#define LEFT_COALESCING 1
#define RIGHT_COALESCING 2

typedef struct MemoryChunkHeader {
    uint64_t size;
    uint8_t state;
    struct MemoryChunkHeader * nextChunk; 
} MemoryChunkHeader;

typedef struct MemoryChunkFooter {
    uint64_t size;
    uint8_t state;
} MemoryChunkFooter;

typedef struct MemoryManagerCDT {
    MemoryChunkHeader * head;
    void * firstAddress;
    uint64_t size;
    MemoryInfoADT info;
} MemoryManagerCDT;

//static MemoryManagerADT getMemoryManager();
static MemoryChunkHeader * assignNewChunk(MemoryChunkHeader * intoChunk, uint64_t sizeRequested);

static MemoryChunkHeader * leftCoalesceFree(MemoryChunkHeader * leftChunkFooter, MemoryChunkHeader * chunkToFreeHeader);
static MemoryChunkHeader * rightCoalesceFree(MemoryChunkHeader * chunkToFreeHeader, MemoryChunkHeader * rightChunkHeader);
static MemoryChunkHeader * noCoalesceFree(MemoryChunkHeader * chunkToFreeHeader);

static uint8_t addressInBounds(void * address);

MemoryManagerADT createMemoryManager(void * const firstAdress, const uint64_t availableMem) {
    MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;

    memoryManager->firstAddress = firstAdress;
    memoryManager->size = availableMem;

    MemoryChunkHeader * firstChunkHeader = (MemoryChunkHeader *) firstAdress;
    firstChunkHeader->size = availableMem - HEADER_FOOTER_SIZE;
    firstChunkHeader->state = FREE;
    firstChunkHeader->nextChunk = NULL;

    MemoryChunkFooter * firstChunkFooter = (MemoryChunkFooter *) (firstAdress +  (availableMem - sizeof(MemoryChunkFooter)));
    firstChunkFooter->size = availableMem - HEADER_FOOTER_SIZE;
    firstChunkFooter->state = FREE;

    memoryManager->head = firstChunkHeader;
    createMemoryInfo(memoryManager->info, availableMem);

    return memoryManager;
}

void * allocMemory(const uint64_t size) {
    if (size <= 0) {
        return NULL;
    }

    MemoryManagerADT memoryManager = getMemoryManager();

    MemoryChunkHeader * currentChunkHeader = memoryManager->head;
    uint64_t sizeToAlloc = size + HEADER_FOOTER_SIZE;
    while(currentChunkHeader != NULL && sizeToAlloc > currentChunkHeader->size) {
        currentChunkHeader = currentChunkHeader->nextChunk;
    }

    if (currentChunkHeader == NULL) {
        return NULL;
    }

    MemoryChunkHeader * newChunkAlloc = assignNewChunk(currentChunkHeader, size);

    allocUpdateInfo(memoryManager->info, size);

    return (void *) newChunkAlloc + HEADER_SIZE;
}

void freeMemory(void * ptrToFree) {
    MemoryChunkHeader * chunkToFree = (MemoryChunkHeader *) (ptrToFree - HEADER_SIZE);

    if (chunkToFree->state == FREE) {
        return;
    }

    MemoryManagerADT memoryManager = getMemoryManager();

    freeUpdateInfo(memoryManager->info, chunkToFree->size);

    MemoryChunkFooter * leftChunkFooter = (MemoryChunkFooter *) ((void *) chunkToFree - FOOTER_SIZE);
    MemoryChunkHeader * leftChunkHeader = (MemoryChunkHeader *) ((void *) chunkToFree - (leftChunkFooter->size + HEADER_FOOTER_SIZE));
    MemoryChunkHeader * rightChunkHeader = (MemoryChunkHeader *) ((void *) chunkToFree + (chunkToFree->size + HEADER_FOOTER_SIZE));
    
    uint8_t decision = NO_COAELESCING;
    if (addressInBounds((void *) rightChunkHeader) && rightChunkHeader->state == FREE) {
        decision = RIGHT_COALESCING;
    }

    if (addressInBounds((void *) leftChunkFooter) && leftChunkFooter->state == FREE) {
        decision = LEFT_COALESCING;
    }

    switch (decision) {
        case LEFT_COALESCING:
            leftCoalesceFree(leftChunkHeader, chunkToFree); 
            break;
        case RIGHT_COALESCING:
            rightCoalesceFree(chunkToFree, rightChunkHeader);
            break;
        case NO_COAELESCING: 
            MemoryChunkHeader * newFreeChunk = noCoalesceFree(chunkToFree); 
            newFreeChunk->nextChunk = memoryManager->head;
            memoryManager->head = newFreeChunk;
            break;
    }

    freeUpdateInfo(memoryManager->info, chunkToFree->size);
}

static MemoryChunkHeader * leftCoalesceFree(MemoryChunkHeader * leftChunkHeader, MemoryChunkHeader * chunkToFreeHeader) {
    leftChunkHeader->size = chunkToFreeHeader->size + leftChunkHeader->size + HEADER_FOOTER_SIZE;
    leftChunkHeader->state = FREE;

    MemoryChunkFooter * newLeftChunkFooter = (MemoryChunkFooter *) ((void *) leftChunkHeader + (leftChunkHeader->size + HEADER_SIZE));
    newLeftChunkFooter->size = leftChunkHeader->size;
    newLeftChunkFooter->state = FREE;

    return leftChunkHeader;
}

static MemoryChunkHeader * rightCoalesceFree(MemoryChunkHeader * chunkToFreeHeader, MemoryChunkHeader * rightChunkHeader) {
    chunkToFreeHeader->size = chunkToFreeHeader->size + rightChunkHeader->size + HEADER_FOOTER_SIZE;
    chunkToFreeHeader->state = FREE;
    chunkToFreeHeader->nextChunk = rightChunkHeader->nextChunk;

    MemoryChunkFooter * newRightChunkFooter = (MemoryChunkFooter *) ((void *) rightChunkHeader + (rightChunkHeader->size + HEADER_SIZE));
    newRightChunkFooter->size = chunkToFreeHeader->size;
    newRightChunkFooter->state = FREE;

    return chunkToFreeHeader;
}

static MemoryChunkHeader * noCoalesceFree(MemoryChunkHeader * chunkToFreeHeader) {
    MemoryChunkFooter * chunkToFreeFooter = (MemoryChunkFooter *) ((void *) chunkToFreeHeader + (chunkToFreeHeader->size+ HEADER_SIZE));
    chunkToFreeFooter->state = FREE;

    chunkToFreeHeader->state = FREE;

    return chunkToFreeHeader;
}

static uint8_t addressInBounds(void * address) {
    MemoryManagerADT memoryManager = getMemoryManager();
    return (address >= memoryManager->firstAddress && address < (memoryManager->firstAddress + memoryManager->size));
}

static MemoryChunkHeader * assignNewChunk(MemoryChunkHeader * intoChunk, uint64_t sizeRequested) {
    MemoryChunkFooter * newChunkFooterToAlloc = (MemoryChunkFooter *) ((void *) intoChunk + (intoChunk->size + HEADER_SIZE));
    newChunkFooterToAlloc->size = sizeRequested;
    newChunkFooterToAlloc->state = IN_USE;

    MemoryChunkHeader * newChunkHeaderToAlloc = (MemoryChunkHeader *) ((void *) newChunkFooterToAlloc - (newChunkFooterToAlloc->size + HEADER_SIZE));
    newChunkHeaderToAlloc->state = IN_USE;
    newChunkHeaderToAlloc->size = sizeRequested;
    newChunkHeaderToAlloc->nextChunk = NULL;

    MemoryChunkFooter * newChunkFooterFree = (MemoryChunkFooter *) ((void *) newChunkHeaderToAlloc - (FOOTER_SIZE));
    newChunkFooterFree->state = FREE;
    newChunkFooterFree->size = intoChunk->size - (sizeRequested + HEADER_FOOTER_SIZE);

    MemoryChunkHeader * newChunkHeaderFree = (MemoryChunkHeader *) ((void *) newChunkFooterFree - (newChunkFooterFree->size + HEADER_SIZE));
    newChunkHeaderFree->state = FREE;
    newChunkHeaderFree->size = intoChunk->size - (sizeRequested + HEADER_FOOTER_SIZE);
    newChunkHeaderFree->nextChunk = intoChunk->nextChunk;

    MemoryManagerADT memoryManager = getMemoryManager();

    if (intoChunk == memoryManager->head) {
        memoryManager->head = newChunkHeaderFree;
    } else {
        intoChunk = newChunkHeaderFree;
    }

    return newChunkHeaderToAlloc;
}

//Despues de borra creo
MemoryManagerADT getMemoryManager() {
    return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

#endif




