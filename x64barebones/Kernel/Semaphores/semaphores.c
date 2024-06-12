// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <semaphores.h>
#include <definitions.h>
#include <scheduler.h>
#include <linkedListADT.h>

extern int64_t _xchg(int32_t *lock, int32_t value);

typedef struct Semaphore {
    uint64_t id;
    int64_t value;
    int32_t lock;
    LinkedListADT processBlockedPids;
} Semaphore;

typedef struct SemaphoreListCDT {
    LinkedListADT semaphores;
    uint64_t idCounter;
} SemaphoreListCDT;


static SemaphoreListADT getSemaphoreManager() {
    return (SemaphoreListADT) SEMAPHORES_MANAGER_ADDRESS;
}

static uint64_t getSemId(Semaphore * sem) {
    return sem->id;
}

uint64_t getNewSemId(){
    SemaphoreListADT semaphoreList = getSemaphoreManager();
    return semaphoreList->idCounter++;
}

static Node * getSemNodeById( uint64_t semId, SemaphoreListADT list ) {
    Node * current = getFirst(list->semaphores);
    while(current != NULL) {
        if( getSemId((Semaphore *) current->data) == semId ) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static uint64_t createSemaphore(uint64_t value, uint64_t semId) {
    SemaphoreListADT semaphoresList = getSemaphoreManager();
    Semaphore * newSem = allocMemory(sizeof(Semaphore));
    if (newSem == NULL) {
        return -1;
    }
    newSem->id = semId ;
    newSem->value = value;
    newSem->lock = 0;
    newSem->processBlockedPids = createLinkedList();
    if (newSem->processBlockedPids == NULL) {
        freeMemory(newSem);
        return -1;
    }
    insert(semaphoresList->semaphores, newSem);
    return newSem->id;
}


void acquire(Semaphore * sem) { 
    while (_xchg(&sem->lock, 1) != 0){
        uint32_t processPid = getCurrentPid();
        void *pidPointer = (void *)(uintptr_t)processPid;
        insert(sem->processBlockedPids, pidPointer);
        setState(processPid, BLOCKED);
        yield(); 
    } 
} 
void release(Semaphore * sem) { 
    _xchg(&sem->lock, 0); 
}

void createSemaphoreManager() {
   SemaphoreListADT semaphoresList = getSemaphoreManager();
   semaphoresList->semaphores = createLinkedList();
   semaphoresList->idCounter = 0;
}

int64_t semOpen(int64_t value, int64_t semId) {
    if(value < 0 || semId < 0) {
        return -1;
    }
    SemaphoreListADT semaphoresList = getSemaphoreManager();

    Node *semNode = getSemNodeById(semId, semaphoresList);
    if (semNode == NULL) { 
        uint64_t id = createSemaphore(value, semId);
        return id;
    }
    return semId;
}

int64_t semWait(int64_t semId) {
    if(semId < 0 ){
        return -1;
    }

    SemaphoreListADT semList = getSemaphoreManager();
    Node * semNode = getSemNodeById(semId, semList);

    Semaphore * sem = semNode->data;

    if(sem == NULL) {
        return -1;
    }

    acquire(sem);

    if(sem->value > 0) {
        sem->value--;
        release(sem);
    } else if(sem->value == 0){ 
        uint32_t processPid = getCurrentPid();
        void *pidPointer = (void *)(uintptr_t)processPid;
        insert(sem->processBlockedPids, pidPointer);
        release(sem);
        setState(processPid, BLOCKED);
        yield();
        return 1;
    } else {
        release(sem);
        return -1;
    }

    return 1;
}

int64_t semPost(int64_t semId) {
    if(semId < 0 ){
        return -1;
    }
    SemaphoreListADT semList = getSemaphoreManager();
    Node * semNode = getSemNodeById(semId, semList);

    Semaphore * sem = semNode->data;

    if(sem == NULL) {
        return -1;
    }

    acquire(sem);

    if(getListSize(sem->processBlockedPids) > 0){
        void *data = removeFirst(sem->processBlockedPids);
            if (data != NULL) {
                int32_t processPid = (int32_t)(intptr_t)data; // Conversión correcta
                if (processPid != -1) {
                    setState(processPid, READY);
                }
        }
        release(sem);
    } else{
        sem->value++;
        release(sem);
    }

    return 1;
}

int8_t semClose(int64_t semId) {
    if (semId < 0) {
        return -1;
    }
    SemaphoreListADT semList = getSemaphoreManager();

    Node *semNode = getSemNodeById(semId, semList);
    if (semNode == NULL) {
        return -1;
    }

    Semaphore *sem = semNode->data;
    if (sem == NULL) {
        return -1;
    }

    if (getListSize(sem->processBlockedPids) != 0) {
        return -1;
    }

    freeMemory(sem->processBlockedPids);
    removeNode(semList->semaphores, semNode);
    freeMemory(sem);
    return 1;
}


