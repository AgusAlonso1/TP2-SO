#include <memoryManager.h>
#include <stdlib.h>
#include <memoryInfoADT.h>

#define BASE (int) 2
#define MIN_EXP 5 // 2^5 = 32 B
#define TRUE 1
#define FALSE 0

#define FREE (uint8_t) 0
#define IN_USE (uint8_t) 1


typedef struct MemoryChunk { // Header of chunks with a size of 18 bytes -> MIN_EXP = 5 (32 bytes)
    uint8_t exp;
    uint8_t state;
    struct MemoryChunk * previousChunk;
    struct MemoryChunk * nextChunk;
} MemoryChunk;

typedef struct MemoryManagerCDT {
    uint8_t maxExpOfTwo;
    void * firstAvailableAdress;
    MemoryChunk * chunks[MAX_EXP + 1]; // Each position represents the power of two.
    MemoryInfoADT info;
} memoryManagerCDT;

static uint8_t log2(uint64_t argument);
//static uint64_t pow2(uint64_t argument);

static int reorderChunks(uint8_t expIndexToAlloc);
static void splitChunk(uint8_t chunkIndex);
static MemoryChunk * joinChunks(MemoryChunk * chunk, MemoryChunk * buddyChunk);
static MemoryChunk * getBuddyChunk(MemoryChunk * chunk);
static MemoryChunk * getBuddyChunk(MemoryChunk * chunk);


static MemoryChunk * createMemoryChunk(void * destinationAdress, uint8_t exp, MemoryChunk * next);
static void * removeChunk(MemoryChunk * chunk);

MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;

MemoryManagerADT createMemoryManager(void * firstAdress, uint64_t const availableMem) {
    MemoryManagerADT memoryManager = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
    memoryManager->maxExpOfTwo = log2(availableMem);
    memoryManager->firstAvailableAdress = firstAdress;

    if (memoryManager->maxExpOfTwo < MIN_EXP) { // Max exp is lower than min possible exp.
        return NULL;
    }

    createMemoryInfo(memoryManager->info, availableMem);

    for (uint8_t current_exp = 0; current_exp <= MAX_EXP; current_exp++) { // Initialize all chunks in NULL
        memoryManager->chunks[current_exp] = NULL;
    }

    memoryManager->chunks[memoryManager->maxExpOfTwo] = createMemoryChunk(firstAdress, memoryManager->maxExpOfTwo, NULL);
    return memoryManager;
}

void *allocMemory(const uint64_t size) {
    uint8_t expToAlloc = log2(size + sizeof(MemoryChunk));
    uint8_t expIndexToAlloc;

    if (expToAlloc >= MAX_EXP) { // Desired size is bigger than the full available memory
        return NULL;
    }

    if (expToAlloc < MIN_EXP ) { // Desired size is smaller than min chunk size
        expIndexToAlloc = MIN_EXP - 1;
    } else {
        expIndexToAlloc = expToAlloc;
    }
    MemoryManagerADT memoryManager = getMemoryManager();

    if(!reorderChunks(expIndexToAlloc)){
        return NULL;
    }

    MemoryChunk * selectedChunk = memoryManager->chunks[expIndexToAlloc];
    removeChunk(selectedChunk);

    selectedChunk->state = IN_USE;
    selectedChunk->nextChunk = NULL;
    selectedChunk->previousChunk = NULL;

    allocUpdateInfo(memoryManager->info, pow2(selectedChunk->exp));

    return (void *) selectedChunk + sizeof(MemoryChunk);
}

void freeMemory(void * ptrToFree) {
    MemoryChunk * chunk = (MemoryChunk *) (ptrToFree - sizeof(MemoryChunk));

    
    if (chunk->state == FREE) {
        return;
    }
    chunk->state = FREE;

    MemoryManagerADT memoryManager = getMemoryManager();

    freeUpdateInfo(memoryManager->info, pow2(chunk->exp));

    MemoryChunk * buddyChunk = getBuddyChunk(chunk);
    while (chunk->exp < memoryManager->maxExpOfTwo && buddyChunk->state == FREE && chunk->exp == buddyChunk->exp) {
        // re-assigned "chunk" and "buddyChunk" values to recursively check if its possible to join chunks up top
        chunk = joinChunks(chunk, buddyChunk);
        buddyChunk = getBuddyChunk(chunk);
    }
    // After the final join, we place the new free block in its new slot
    memoryManager->chunks[chunk->exp - 1] = createMemoryChunk((void *) chunk, chunk->exp, memoryManager->chunks[chunk->exp - 1]);

}

MemoryManagerADT getMemoryManager() {
    return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

static uint8_t log2(uint64_t argument) {
    unsigned int count = 0;
    uint64_t value = 1;
    while (value < argument) {
        value *= BASE;
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
    uintptr_t mask = (uintptr_t) (1L << (chunk->exp));
    uintptr_t copyOfAddress = (uintptr_t) chunk;

    uintptr_t buddyAdress = copyOfAddress ^ mask;

    return (MemoryChunk *) buddyAdress;
}

static int reorderChunks(uint8_t expIndexToAlloc) {
    MemoryManagerADT memoryManager = getMemoryManager();
    char isPossible = 0;
    if (memoryManager->chunks[expIndexToAlloc] == NULL) { // No chunk of the exponent size, need to split bigger ones
        uint8_t possibleIndex;
        for (possibleIndex = expIndexToAlloc + 1; possibleIndex < (memoryManager->maxExpOfTwo); possibleIndex++) {
            if (memoryManager->chunks[possibleIndex] != NULL) { // Found the closest available chunk
                isPossible = 1;
               break;
            }
        }

        if(possibleIndex == memoryManager->maxExpOfTwo && memoryManager->chunks[memoryManager->maxExpOfTwo] != NULL){
            isPossible = 1;
        }

        while (possibleIndex > expIndexToAlloc && isPossible) { // Split all blocks from the first available to the desired one
            splitChunk(possibleIndex);
            possibleIndex--;
        } 
    }else {
        isPossible = 1;
    }

    return isPossible;
}

static void splitChunk(uint8_t chunkIndex) {
    MemoryManagerADT memoryManager = getMemoryManager();

    void * adress = removeChunk(memoryManager->chunks[chunkIndex]);

    void * buddyAdress = adress + (1L << (chunkIndex - 1));
    MemoryChunk * buddyChunk = createMemoryChunk(buddyAdress, chunkIndex - 1, NULL);
    memoryManager->chunks[chunkIndex - 1] = createMemoryChunk(adress, chunkIndex - 1, buddyChunk);
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
    uint8_t chunksIndex = chunk->exp;
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
