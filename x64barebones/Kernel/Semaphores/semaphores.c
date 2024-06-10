#include <semaphores.h>
#include <definitions.h>
#include <scheduler.h>
#include <linkedListADT.h>

extern int64_t enter_region(uint8_t *lock);
extern void leave_region(uint8_t *lock);

typedef struct Semaphore {
    uint64_t id;
    int64_t value;
    uint32_t lock;
    LinkedListADT processBlockedPids;
} Semaphore;

typedef struct SemaphoreListCDT {
    LinkedListADT semaphores;
    uint8_t globalLock;
} SemaphoreListCDT;


static SemaphoreListADT getSemaphoreManager() {
    return (SemaphoreListADT) SEMAPHORES_MANAGER_ADDRESS;
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
    leave_region(&(sem->lock));
    Node * currentNode = getFirst(sem->processBlockedPids);
    while(currentNode != NULL) {
        void *data = removeFirst(sem->processBlockedPids);
        if (data != NULL) {
            uint32_t processPid = (uint32_t)(uintptr_t)data; // Conversión correcta
            if (processPid != -1) {
                setState(processPid, READY);
            }
        }
        currentNode = currentNode->next;
    }
}

void createSemaphoreManager() {
   SemaphoreListADT semaphoresList = getSemaphoreManager();
   semaphoresList->semaphores = createLinkedList();
   semaphoresList->globalLock = 0;
}

int64_t semOpen(uint64_t value, uint64_t semId) {
    if(value < 0 || semId < 0) {
        return -1;
    }
    uint32_t p = getCurrentPid(); //eliminar
    SemaphoreListADT semaphoresList = getSemaphoreManager();
    while (enter_region(&(semaphoresList->globalLock))) {
        yield();
    }
    Node *semNode = getSemNodeById(semId, semaphoresList);
    if (semNode == NULL) { // No existe
        uint64_t id = createSemaphore(value, semId);
        leave_region(&(semaphoresList->globalLock)); // Libera el bloqueo
        return id;
    }
    Semaphore * sem = (Semaphore *)semNode->data;
    leave_region(&(semaphoresList->globalLock)); // Libera el bloqueo
    return semId;
}

int64_t semWait(uint64_t semId) {
    if(semId < 0 ){
        return -1;
    }

    uint32_t p = getCurrentPid(); //eliminar
    SemaphoreListADT semList = getSemaphoreManager();
    Node * semNode = getSemNodeById(semId, semList);

    Semaphore * sem = semNode->data;

    if(sem == NULL) {
        return -1;
    }

    spinLock(sem);

    if(sem->value > 0) {
        sem->value--;
    } else if(sem->value == 0){ 
        uint32_t processPid = getCurrentPid();
        void *pidPointer = (void *)(uintptr_t)processPid;
        insert(sem->processBlockedPids, pidPointer);
        setState(processPid, BLOCKED);
        unlock(sem);
        yield();
        spinLock(sem);
    } else {
        unlock(sem);
        return -1;
    }

    unlock(sem);

    return 1;
}

int64_t semPost(uint64_t semId) {
    if(semId < 0 ){
        return -1;
    }
    uint32_t p = getCurrentPid(); //eliminar
    SemaphoreListADT semList = getSemaphoreManager();
    Node * semNode = getSemNodeById(semId, semList);

    Semaphore * sem = semNode->data;

    if(sem == NULL) {
        return -1;
    }

    spinLock(sem);

    if(sem->value < 0){
        unlock(sem->lock);
        return -1;
    }

    sem->value++;

    unlock(sem);
    return 1;
}

int8_t semClose(uint64_t semId) {
    if (semId < 0) {
        return -1;
    }
    SemaphoreListADT semList = getSemaphoreManager();

    while (enter_region(&(semList->globalLock))) {
        yield();
    }

    Node *semNode = getSemNodeById(semId, semList);
    if (semNode == NULL) {
        leave_region(&(semList->globalLock));
        return -1;
    }

    Semaphore *sem = semNode->data;
    if (sem == NULL) {
        leave_region(&(semList->globalLock));
        return -1;
    }

    if (getListSize(sem->processBlockedPids) != 0) {
        leave_region(&(semList->globalLock));
        return -1; // Hay procesos esperando en el semáforo
    }

    freeMemory(sem->processBlockedPids);
    removeNode(semList->semaphores, semNode);
    freeMemory(sem);

    leave_region(&(semList->globalLock));

    return 1;
}
