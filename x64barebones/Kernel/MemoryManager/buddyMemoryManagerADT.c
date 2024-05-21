#include <memoryManager.h>
#include <stdlib.h>
#include <memoryInfoADT.h>

#define BASE 2
#define MIN_EXP 5 // 2^5 = 32 B
#define TRUE 1
#define FALSE 0

typedef enum {
    FREE,
    IN_USE
} ChunkState;

typedef struct MemoryChunk { // Header of chunks with a size of 18 bytes -> MIN_EXP = 5 (32 bytes)
    uint8_t exp;
    ChunkState state;
    struct MemoryChunk * previousChunk;
    struct MemoryChunk * nextChunk;
} MemoryChunk;

typedef struct MemoryManagerCDT {
    uint8_t maxExpOfTwo;
    void * firstAvailableAdress;
    MemoryChunk * chunks[MAX_EXP]; // Each position represents the power of two.
    MemoryInfoADT info;
} memoryManagerCDT;

static uint8_t log2(uint64_t argument);
//static uint64_t pow2(uint64_t argument);

static MemoryManagerADT getMemoryManager();
static void reorderChunks(uint8_t expIndexToAlloc);
static void splitChunk(uint8_t chunkIndex);
static MemoryChunk * joinChunks(MemoryChunk * chunk, MemoryChunk * buddyChunk);
static MemoryChunk * getBuddyChunk(MemoryChunk * chunk);
static MemoryChunk * getBuddyChunk(MemoryChunk * chunk);


static MemoryChunk * createMemoryChunk(void * destinationAdress, uint8_t exp, MemoryChunk * next);
static void * removeChunk(MemoryChunk * chunk);


MemoryManagerADT createMemoryManager(void * const firstAdress, uint64_t const availableMem) {
    MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
    memoryManager->maxExpOfTwo = log2(availableMem);
    memoryManager->firstAvailableAdress = firstAdress;

    if (memoryManager->maxExpOfTwo < MIN_EXP) { // Max exp is lower than min possible exp.
        return NULL;
    }

    createMemoryInfo(memoryManager->info, availableMem);

    for (uint8_t current_exp = 0; current_exp < MAX_EXP; current_exp++) { // Initialize all chunks in NULL
        memoryManager->chunks[current_exp] = NULL;
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

    reorderChunks(expIndexToAlloc);
    MemoryChunk * selectedChunk = memoryManager->chunks[expIndexToAlloc];
    removeChunk(selectedChunk);

    selectedChunk->state = IN_USE;
    selectedChunk->nextChunk = NULL;
    selectedChunk->previousChunk = NULL;

    allocUpdateInfo(memoryManager->info, pow2(selectedChunk->exp));

    return (void *) (selectedChunk + sizeof(MemoryChunk));
}

void freeMemory(const void * ptrToFree) {
    MemoryChunk * chunk = (MemoryChunk *) (ptrToFree - sizeof(MemoryChunk));
    if (chunk->state == FREE) {
        return;
    }
    chunk->state = FREE;

    MemoryManagerADT memoryManager = getMemoryManager();

    freeUpdateInfo(memoryManager->info, pow2(chunk->exp));

    MemoryChunk * buddyChunk = getBuddyChunk(chunk);
    while (chunk->exp < memoryManager->maxExpOfTwo && buddyChunk->exp == FREE /*&& chunk->exp == buddyChunk->exp*/) {
        // re assigned "chunk" and "buddyChunk" values to recursively check if its possible to join chunks up top
        chunk = joinChunks(chunk, buddyChunk);
        buddyChunk = getBuddyChunk(chunk);
    }
    // After the final join, we place the new free block in its new slot
    memoryManager->chunks[chunk->exp - 1] = createMemoryChunk((void *) chunk, chunk->exp, memoryManager->chunks[chunk->exp - 1]);

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

uint64_t pow2(uint64_t argument) {
    uint64_t count = 1;
    while (argument > 0) {
        count *= BASE;
        argument--;
    }
    return count;
}

static MemoryChunk * joinChunks(MemoryChunk * chunk, MemoryChunk * buddyChunk) {
    removeChunk(buddyChunk);
    MemoryChunk * bigChunkAddress = (chunk < buddyChunk) ? chunk : buddyChunk;
    bigChunkAddress->exp++;

    return bigChunkAddress;
}

static MemoryChunk * getBuddyChunk(MemoryChunk * chunk) {
    uintptr_t mask = (uintptr_t) (1L << (chunk->exp - 1));
    uintptr_t copyOfAddress = (uintptr_t) chunk;

    uintptr_t buddyAdress = copyOfAddress ^ mask;

    return (MemoryChunk *) buddyAdress;
}

static void reorderChunks(uint8_t expIndexToAlloc) {
    MemoryManagerADT memoryManager = getMemoryManager();
    if (memoryManager->chunks[expIndexToAlloc] == NULL) { // No chunk of the exponent size, need to split bigger ones
        uint8_t availableChunkIdx = FALSE;
        for (uint8_t possibleIndex = expIndexToAlloc + 1; possibleIndex < MAX_EXP - 1 && !availableChunkIdx; possibleIndex++) {
            if (memoryManager->chunks[possibleIndex] != NULL) { // Found the closest available chunk
                availableChunkIdx = TRUE;
            }
        }
        while (availableChunkIdx > expIndexToAlloc) { // Split all blocks from the first available to the desired one
            splitChunk(availableChunkIdx);
            availableChunkIdx--;
        } 
    }

}

static void splitChunk(uint8_t chunkIndex) {
    MemoryManagerADT memoryManager = getMemoryManager();

    void * adress = removeChunk(memoryManager->chunks[chunkIndex]);

    void * buddyAdress = adress + (1L << chunkIndex);
    MemoryChunk * buddyChunk = createMemoryChunk(buddyAdress, chunkIndex, NULL);
    memoryManager->chunks[chunkIndex - 1] = createMemoryChunk(adress, chunkIndex, buddyChunk);
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

// Removes the first chunk of the exp and returns its adress
static void * removeChunk(MemoryChunk * chunk) {
    MemoryManagerADT memoryManager = getMemoryManager();
    uint8_t chunksIndex = chunk->exp - 1;
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
