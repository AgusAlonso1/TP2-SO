#include <memoryManager.h>

#include <stdlib.h>
#include <memoryInfoADT.h>

#define MAX_EXP 30 // 2^30 = 1 GB
#define BASE 2
#define MIN_EXP 5
#define TRUE 1

static uint8_t log2(uint64_t argument);
static uint64_t pow2(uint64_t argument);

static MemoryManagerADT getMemoryManager();
static reorderBlocks(uint8_t expIndexToAlloc);
static void splitChunk(MemoryChunk * chunk, MemoryManagerADT memoryManager);


static MemoryChunk * createMemoryChunk(void * destinationAdress, uint8_t exp, MemoryChunk * next);
static void * removeChunk(uint8_t chunksIndex);

typedef enum {
    FREE,
    IN_USE
} ChunkState;

typedef struct MemoryChunk { // Header of chunks with a size of 18 bytes -> MIN_EXP = 5 (32 bytes)
    uint8_t exp;
    ChunkState state;
    MemoryChunk * previousChunk;
    MemoryChunk * nextChunk;
} MemoryChunk;

typedef struct MemoryManagerCDT {
    uint8_t maxExpOfTwo;
    void * firstAvailableAdress;
    MemoryChunk * chunks[MAX_EXP]; // Each position represents the power of two.
    MemoryInfoADT info;
} memoryManagerCDT;


MemoryManagerADT createMemoryManager(void *const firstAdress, uint64_t const availableMem) {
    MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
    memoryManager->maxExpOfTwo = log2(availableMem);
    memoryManager->firstAvailableAdress = firstAdress;

    if (memoryManager->maxExpOfTwo < MIN_EXP) { // Max exp is lower than min possible exp.
        return NULL;
    }

    createMemoryInfo(memoryManager->info, availableMem);

    for (uint8_t i = 0; i < MAX_EXP; i++) { // Initialize all chunks in NULL
        memoryManager->chunks[i] = NULL;
    }

    memoryManager->chunks[memoryManager->maxExpOfTwo - 1] = createMemoryChunk(firstAdress, memoryManager->maxExpOfTwo, NULL);
    return memoryManager;
}

void *allocMemory(const uint64_t size) {
    uint8_t expToAlloc = log2(size + sizeof(MemoryChunk));
    uint8_t expIndexToAlloc;

    if (expToAlloc > MAX_EXP) { // Desired size is bigger than the full available memory
        return NULL;
    }

    if (expToAlloc < MIN_EXP ) { // Desired size is smaller than min chunk size
        expIndexToAlloc = MIN_EXP - 1;
    } else {
        expIndexToAlloc = expToAlloc - 1;
    }
    MemoryManagerADT memoryManager = getMemoryManager();

    reorderBlocks(expIndexToAlloc);
    MemoryChunk * selectedChunk = memoryManager->chunks[expIndexToAlloc];
    removeChunk(expIndexToAlloc);

    selectedChunk->state = IN_USE;
    selectedChunk->nextChunk = NULL;
    selectedChunk->previousChunk = NULL;

    allocUpdateInfo(memoryManager->info, pow2(selectedChunk->exp));

    return (void *) (selectedChunk + sizeof(MemoryChunk));
}

void freeMemory(const void * ptrToFree) {
    
}

static MemoryManagerADT getMemoryManager() {
    return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

static uint8_t log2(uint64_t argument) {
    uint8_t count = 0;
    while (argument /= BASE) {
        count++;
    }
    return count;
}

static uint64_t pow2(uint64_t argument) {
    uint64_t count = 1;
    while (argument > 0) {
        count *= BASE;
        argument--;
    }
    return count;
}

static reorderBlocks(uint8_t expIndexToAlloc) {
    MemoryManagerADT memoryManager = getMemoryManager();
    if (memoryManager->chunks[expIndexToAlloc] == NULL) { // No chunk of the exponent size, need to split bigger ones
        uint8_t availableChunkIdx = 0;
        for (uint8_t possibleIndex = expIndexToAlloc + 1; possibleIndex < MAX_EXP - 1 && !availableChunkIdx; possibleIndex++) {
            if (memoryManager->chunks[possibleIndex] != NULL) { // Found the closest available chunk
                availableChunkIdx = TRUE;
            }
        }
        while (availableChunkIdx > expIndexToAlloc) { // Split all blocks from the first available to the desired one
            splitChunk(availableChunkIdx, memoryManager);
            availableChunkIdx--;
        } 
    }

}

static void splitChunk(MemoryChunk * chunk, MemoryManagerADT memoryManager) {
    void * adress = removeChunk(chunk->exp - 1);

    void * secondBuddyAdress = adress + (1L << chunk->exp);
    MemoryChunk * secondBuddyChunk = createMemoryChunk(secondBuddyAdress, chunk->exp, NULL);
    MemoryChunk * firstBuddyChunk = createMemoryChunk(adress, chunk->exp, secondBuddyAdress);
}

static MemoryChunk * createMemoryChunk(void * destinationAdress, uint8_t exp, MemoryChunk * next) {
    MemoryChunk * newChunk = (MemoryChunk *) destinationAdress;
    newChunk->exp = exp;
    newChunk->state = FREE;
    newChunk->previousChunk = NULL;
    newChunk->nextChunk = next;
    if (next != NULL) {
        next->previousChunk = newChunk;
    }

    return newChunk;
}

// Removes the first chunk of the exp adn returns its adress
static void * removeChunk(uint8_t chunksIndex) {
    MemoryManagerADT memoryManager = getMemoryManager();
    MemoryChunk * firstExpChunk = memoryManager->chunks[chunksIndex];

    if (firstExpChunk->previousChunk != NULL) {
        firstExpChunk->previousChunk->nextChunk = firstExpChunk->nextChunk;
    } else {
        memoryManager->chunks[chunksIndex] = firstExpChunk->nextChunk; 
    }

    if (firstExpChunk->nextChunk != NULL) {
        firstExpChunk->nextChunk->previousChunk = firstExpChunk->previousChunk;
    }
    return (void *) firstExpChunk;
}
