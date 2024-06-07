#include <semaphores.h>
#include <definitions.h>
#include <scheduler.h>
#include <linkedListADT.h>

extern uint64_t enter_region(uint64_t *lock);
extern void leave_region(uint64_t *lock);

static uint64_t globalIdCounter = 0;
static uint64_t globalIdLock = 0;

typedef struct Semaphore {
    uint64_t id;
    uint64_t value;
    uint64_t lock;
    LinkedListADT processBlockedPids;
} Semaphore;

typedef struct SemaphoreListCDT {
    LinkedListADT semaphores;
} SemaphoreListCDT;


static SemaphoreListADT getSemaphoreManager() {
    return (SemaphoreListADT) SEMAPHORES_MANAGER_ADDRESS;
}

static uint64_t generateId() {
    uint64_t uniqueId;
    while (enter_region(&globalIdLock));
    uniqueId = ++globalIdCounter;
    leave_region(&globalIdLock);
    return uniqueId;
}	

static uint64_t createSemaphore(uint64_t value) {
    SemaphoreListADT semaphoresList = getSemaphoreManager();
    Semaphore * newSem = allocMemory(sizeof(semaphoresList));
    if((newSem->id = generateId()) == 0) { return -1; } ;
    newSem->value = value;
    newSem->lock = 0;
    newSem->processBlockedPids = createLinkedList();
    insert(semaphoresList->semaphores, newSem);
    return newSem->id;
}

static uint64_t getSemId(Semaphore * sem) {
    return sem->id;
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

static void spinLock(Semaphore * sem){
    while (enter_region(&(sem->lock))) {
        uint32_t processPid = getCurrentPid();
        void *pidPointer = (void *)(uintptr_t)processPid;
        insert(sem->processBlockedPids, pidPointer);
        setState(processPid, BLOCKED);
        yield(); 
    }
}

static void unlock(Semaphore * sem) {
    Node * currentNode;
    while((currentNode = getFirst(sem->processBlockedPids)) != NULL) {
        void *data = removeFirst(sem->processBlockedPids);
        if (data != NULL) {
            uint32_t processPid = (uint32_t)(uintptr_t)data; // Conversión correcta
            if (processPid != -1) {
                setState(processPid, READY);
            }
        }
    }

    leave_region(&(sem->lock));
}

void createSemaphoreManager() {
   SemaphoreListADT semaphores = getSemaphoreManager();
}

uint64_t semOpen(uint64_t value) {
    return createSemaphore(value);
}

uint64_t semWait(uint64_t semId) {
    SemaphoreListADT semList = getSemaphoreManager();
    Node * semNode = getSemNodeById(semId, semList);

    Semaphore * sem = semNode->data;

    if(sem == NULL) {
        return -1;
    }

    spinLock(sem);

    sem->value--;
    if(sem->value < 0) {
        uint32_t processPid = getCurrentPid();
        void *pidPointer = (void *)(uintptr_t)processPid;
        insert(sem->processBlockedPids, pidPointer);
        setState(processPid, BLOCKED);
        unlock(sem);
        yield();
        spinLock(sem);
    }

    unlock(sem);

    return 1;
}

uint64_t semPost(uint64_t semId) {
    SemaphoreListADT semList = getSemaphoreManager();
    Node * semNode = getSemNodeById(semId, semList);

    Semaphore * sem = semNode->data;

    if(sem == NULL) {
        return -1;
    }

    enter_region(&sem->lock);

    sem->value++;

    unlock(sem);
    return 1;
}

uint8_t semClose(uint64_t semId) {
    SemaphoreListADT semList = getSemaphoreManager();
    Node * semNode = getSemNodeById(semId, semList);

    Semaphore * sem = semNode->data;

    if(sem == NULL) {
        return -1;
    }

    if(getListSize(sem->processBlockedPids) != 0){
        return -1; //processes wating for sem
    }

    freeMemory(sem->processBlockedPids);
    removeNode(semList->semaphores, semNode);
    freeMemory(sem);
    freeMemory(semNode);

    return 1;
}