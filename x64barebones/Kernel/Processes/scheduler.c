#include <scheduler.h>

/* Comentarios scheduler:
 *
 */

// TAREAS IMPORTANTES PENDIENTES
//ver logica de backgournd y foreground

typedef struct SchedulerCDT{
    LinkedListADT allProcesses;
    LinkedListADT processes[PRIORITY_LEVELS];
    uint32_t currentPid;
    uint32_t pidCounter;
    uint16_t processQuantum;
    uint64_t processQty;
    char killCurrentProcess;
 //  char blockCurrentProcess;
} SchedulerCDT;

typedef struct ProcessSchedCDT{
    ProcessADT processData;
    uint8_t  quantumWaiting;
} ProcessSchedCDT;

static int quantumLevel[PRIORITY_LEVELS] = {0, 2, 3, 4, 5};

void createScheduler() {
    SchedulerADT sched = (SchedulerADT) SCHEDULER_ADDRESS;
    sched->allProcesses = createLinkedList();
    for(int i = 0; i < PRIORITY_LEVELS; i++){
        sched->processes[i] = createLinkedList();
    }
    sched->currentPid = -1;
    sched->pidCounter = 0;
    sched->processQty = 0;
    sched->killCurrentProcess = 0;
    sched->processQuantum = 0;
}


void * schedule(void * currentStackPointer) {
    SchedulerADT sched = getScheduler();

    uint64_t currentState = BLOCKED;
    ProcessADT oldProcess = getCurrentProcess();
    if(oldProcess != NULL){
        setProcessStack(oldProcess, currentStackPointer);
        currentState = getProcessState(oldProcess);
    }

    if(sched->processQuantum > 0) {
        if (currentState != BLOCKED && currentState != ZOMBIE){
            sched->processQuantum--;
            return currentStackPointer;
        }
    }

    ProcessADT processToRun = NULL;
    if(sched->processQuantum == 0 || currentState == BLOCKED || currentState == ZOMBIE) {

        uint8_t found = 0; 
        for(int i = LEVEL4; i > LEVEL0; i--) {
            Node * currentNode = getFirst(sched->processes[i]);
            while(currentNode != NULL) {
                ProcessSchedADT processSched = (ProcessSchedADT) currentNode->data;
                if (processSched != NULL) {
                    char flag = 0;
                    if (getProcessState(processSched->processData) == READY) {
                        if (!found) {
                            processToRun = processSched->processData;
                            found = 1;
                        } else {
                            processSched->quantumWaiting++;
                            if (processSched->quantumWaiting == 10) { //El quantum se puede cambiar
                                processSched->quantumWaiting = 0;
                                uint64_t priority = getProcessPriority(processSched->processData);
                                if (priority < LEVEL3) {
                                    currentNode = currentNode->next;
                                    flag = 1;
                                    setPriority(getProcessPid(processSched->processData), priority + 1); //este me hace el free del nodo y del processSched
                                }
                            }
                        }
                    }
                    if(!flag){
                        currentNode = currentNode->next;
                    }
                }
            }
        }

        if(oldProcess != NULL) {
            uint64_t oldProcessPriority  = getProcessPriority(oldProcess);
            if(oldProcessPriority != LEVEL1 && getProcessPid(oldProcess) != SHELL) {
                oldProcessPriority--;
            }
            setPriority(getProcessPid(oldProcess), oldProcessPriority);
            if(getProcessState(oldProcess) == RUNNING){
                setProcessState(oldProcess, READY);
            }
        }

        sched->currentPid = getProcessPid(processToRun);
        sched->processQuantum = quantumLevel[getProcessPriority(processToRun)];
        setProcessState(processToRun, RUNNING);

        if(sched->killCurrentProcess && !getProcessMortality(processToRun)){
            if(killProcess(getProcessPid(oldProcess)) != ERROR){
                forceTimerTick();
            }
        }

    }

    return getProcessStack(processToRun);
}

SchedulerADT getScheduler() {
    return (SchedulerADT) SCHEDULER_ADDRESS;
}

uint32_t createProcessFromSched(char* name, char position, uint64_t priority, Function function, char **args, uint32_t parentPid, char mortality) {
    SchedulerADT sched = getScheduler();
    ProcessADT newProcess = NULL;

    newProcess = createProcess(parentPid, sched->pidCounter++, name, priority, mortality, position, function, args);

    if(newProcess != NULL){
        ProcessSchedADT processSched = createProcessSched(newProcess);
        listProcess(processSched);
        insert(sched->allProcesses, newProcess);
        sched->processQty++;
        return getProcessPid(newProcess);
    }

    return -1;
}

void listProcess(ProcessSchedADT processSched) {
    SchedulerADT sched = getScheduler();
    if(processSched == NULL){
        return;
    }
    uint32_t priority = getProcessPriority(processSched->processData);
    insert(sched->processes[priority], processSched);
}


uint64_t setPriority(uint32_t pid, uint64_t priority){
    SchedulerADT sched = getScheduler();

    Node * processNode = getProcessNode(pid);
    if(processNode == NULL || processNode->data == NULL || priority >= PRIORITY_LEVELS){
        return ERROR;
    }
    ProcessSchedADT processSched = (ProcessSchedADT) processNode->data;
    if(processSched->processData == NULL){
        return ERROR;
    }

    uint64_t oldPriority = getProcessPriority(processSched->processData);

    if(oldPriority == priority){
        return SUCCESS;
    }

    ProcessADT process = processSched->processData;
    removeNode(sched->processes[oldPriority], processNode);

    setProcessPriority(process, priority);
    listProcess(processSched);

    return SUCCESS;
}

void yield(){
    SchedulerADT sched = getScheduler();
    sched->processQuantum = 0;
    forceTimerTick();
}

uint64_t killProcess(uint32_t pid){
    SchedulerADT sched = getScheduler();

    Node * processNode = getProcessNode(pid);
    if(processNode == NULL || processNode->data == NULL) {
        return ERROR;
    }

    ProcessSchedADT processSched = (ProcessSchedADT) processNode->data;
    if(processSched->processData == NULL || getProcessMortality(processSched->processData) || getProcessState(processSched->processData) == ZOMBIE){
        return ERROR;
    }

    ProcessADT processToKill = processSched->processData;

    Node * current = getFirst(sched->allProcesses);
    while(current != NULL){
        if(getProcessParentPid(current->data) == pid){
            Node * aux = current;
            current = current->next;
            if(getProcessState(aux->data) != ZOMBIE){
                Node * node = getProcessNode(getProcessPid(aux->data));
                if(node != NULL && node->data != NULL){
                    freeProcessSched(node->data);
                    removeNode(sched->processes[getProcessPriority(processToKill)], node);
                }
            }
            removeFromAllProcesses(getProcessPid(aux->data));
        } else{
            current = current->next;
        }
    }

    Node * parent = getProcessNode(getProcessParentPid(processToKill));
    if(parent != NULL && parent->data != NULL) {
        ProcessSchedADT parentProcessSched = (ProcessSchedADT) parent->data;
        if(parentProcessSched->processData != NULL) {
            if (getProcessState(parentProcessSched->processData) != ZOMBIE) {
                if (getProcessWaitingPid(parentProcessSched->processData) == getProcessPid(processToKill) &&
                    getProcessState(parentProcessSched->processData) == BLOCKED) {
                    setState(getProcessPid(parentProcessSched->processData), READY);
                } else{
                    freeProcessSched(processSched);
                    removeNode(sched->processes[getProcessPriority(processToKill)], processNode);
                    setProcessState(processToKill, ZOMBIE);
                }
            } else {    //aca ni me caliento en ponerlo ZOMBIE, total lo voy a eliminar de todos lados
                freeProcessSched(processSched);
                removeNode(sched->processes[getProcessPriority(processToKill)], processNode);
                removeFromAllProcesses(pid);
            }
        }
    }



    if(sched->currentPid == pid){
        yield();
    }

    return SUCCESS;
}

uint16_t setState(uint32_t pid, uint64_t state) {
    SchedulerADT sched = getScheduler();

    Node * processNode = getProcessNode(pid);
    if(processNode == NULL || processNode->data == NULL){
        return ERROR;
    }
    ProcessSchedADT processSched = (ProcessSchedADT) processNode->data;
    if(processSched->processData == NULL){
        return ERROR;
    }

    uint64_t oldState = getProcessState(processSched->processData);

    if(state == RUNNING || oldState == ZOMBIE || state == ZOMBIE){
        return ERROR;
    } else if(oldState == state){
        return SUCCESS;
    }

    setProcessState(processSched->processData, state);

    if(state == BLOCKED){
        removeNode(sched->processes[getProcessPriority(processSched->processData)],processNode);
        setProcessPriority(processSched->processData, LEVEL4);
        listProcess(processSched);
    } else if(oldState == BLOCKED){
        sched->processQuantum = 0;
    }
    return SUCCESS;
}

Node * getProcessNode(uint32_t pid){
    SchedulerADT sched = getScheduler();
    Node * processNode = NULL;
    uint8_t found = 0;
    for(int i = LEVEL4; i > LEVEL0; i--) {
        Node * currentNode = getFirst(sched->processes[i]);
        while(currentNode != NULL && !found) {
            ProcessSchedADT processSched = (ProcessSchedADT) currentNode->data;
            if(getProcessPid(processSched->processData) == pid) {
                processNode = currentNode;
                found = 1;
            }
            currentNode = currentNode->next;
        }
    }
    return processNode;
}

uint32_t getCurrentPid(){
    SchedulerADT sched = getScheduler();
    return sched->currentPid;
}

uint32_t getCurrentParentPid(){
    ProcessADT current = getCurrentProcess();
    if(current == NULL){
        return ERROR;
    }
    return getProcessParentPid(current);
}


void exitProcess(int returnValue){
    ProcessADT current = getCurrentProcess();
    if(current == NULL){
        return;
    }
    setProcessReturnValue(current, returnValue);
    killProcess(getCurrentPid());
}

uint64_t waitProcessPid(uint32_t pid) {
    SchedulerADT sched = getScheduler();
    Node * childNode =  getProcessNode(pid);
    if(childNode == NULL) {
        return ERROR;
    }
    ProcessSchedADT processSched = (ProcessSchedADT) childNode->data;
    if(processSched == NULL){
        return ERROR;
    }

    ProcessADT childProcess = processSched->processData;
    uint32_t parentPid = getProcessParentPid(childProcess);
    if(parentPid != getCurrentPid()) {
        return ERROR;
    }

    ProcessADT current = getCurrentProcess();
    setProcessWaitingPid(current, pid);
    if(getProcessState(childProcess) != ZOMBIE) {
        setState(sched->currentPid, BLOCKED);
        yield();
    }
    uint64_t toReturn = getProcessReturnValue(childProcess);

    setProcessState(childProcess, ZOMBIE);
    freeProcessSched(processSched);
    removeNode(sched->processes[getProcessPriority(childProcess)], childNode);

    return toReturn;
}

ProcessCopyList * getProcessCopy(){
    SchedulerADT sched = getScheduler();
    ProcessCopyList * processListCopies = allocMemory(sizeof(ProcessCopyList));
    ProcessCopy * processCopyArray =  allocMemory(sched->processQty * sizeof(ProcessCopy));
    int index = 0;

    Node * currentNode = getFirst(sched->allProcesses);
    while(index < sched->processQty) {
        ProcessADT process = currentNode->data;
        copyProcess(&processCopyArray[index], process);
        index++;
        currentNode = currentNode->next;
    }
    processListCopies->length = index;
    processListCopies->processCopyList = processCopyArray;


    return processListCopies;
}
void freeProcessCopy(ProcessCopyList * processCopyList){
    SchedulerADT sched = getScheduler();

    Node * currentNode = getFirst(sched->allProcesses);
    int i = 0;
    while(i < processCopyList->length) {
        freeMemory(processCopyList->processCopyList[i].name);
    }

    freeMemory(processCopyList->processCopyList);
    freeMemory(processCopyList);
}


ProcessSchedADT createProcessSched(ProcessADT process){
    ProcessSchedADT processSched = allocMemory(sizeof (ProcessSchedCDT));
    processSched->processData = process;
    processSched->quantumWaiting = 0;
    return processSched;
}

void freeProcessSched(ProcessSchedADT processSched){
    freeMemory(processSched);
}


ProcessADT getCurrentProcess(){
    SchedulerADT sched = getScheduler();
    Node * current = getProcessNode(sched->currentPid);
    ProcessADT process = NULL;
    if(current != NULL){
        ProcessSchedADT currentProcessSched = (ProcessSchedADT) current->data;
        process = currentProcessSched->processData;
    }
    return process;
}

void removeFromAllProcesses(uint32_t pid){
    SchedulerADT sched = getScheduler();
    Node * processNode = NULL;
    ProcessADT process = NULL;
    uint8_t found = 0;
    Node * currentNode = getFirst(sched->allProcesses);
    while(currentNode != NULL && !found) {
        process = (ProcessADT) currentNode->data;
        if(getProcessPid(process) == pid) {
            processNode = currentNode;
            found = 1;
        }
        currentNode = currentNode->next;
    }
    if(processNode != NULL && process != NULL){
        removeNode(sched->allProcesses, processNode);
        sched->processQty--;
        freeProcess(process);
    }
}

