#include <scheduler.h>

/* Comentarios scheduler:
 *
 */

// TAREAS IMPORTANTES PENDIENTES
//ver logica de backgournd y foreground

typedef struct SchedulerCDT{
    LinkedListADT processes[PRIORITY_LEVELS];
    ProcessADT currentProcess;
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
static Node * getDeadChildNode(LinkedListADT list, uint32_t pid);

void createScheduler() {
    SchedulerADT sched = (SchedulerADT) SCHEDULER_ADDRESS;
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
    ProcessADT currentProcess = getCurrentProcess();
    if(currentProcess != NULL){
        setProcessStack(currentProcess, currentStackPointer);
        currentState = getProcessState(currentProcess);
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

        ProcessADT oldProcess = getCurrentProcess();

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

uint32_t createProcessFromSched(char* name, char position, uint64_t priority, Function function, char **args, uint32_t parentPid) {
    SchedulerADT sched = getScheduler();
    ProcessADT newProcess = NULL;

    newProcess = createProcess(parentPid, sched->pidCounter++, name, priority, 0, position, function, args);

    if(newProcess != NULL){
        listProcess(newProcess);
        sched->processQty++;
        return getProcessPid(newProcess);
    }

    return -1;
}

void listProcess(ProcessADT process) {
    SchedulerADT sched = getScheduler();
    if(process == NULL){
        return;
    }
    ProcessSchedADT processSched = createProcessSched(process);
    uint32_t priority = getProcessPriority(process);
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
    freeProcessSched(processSched);

    setProcessPriority(process, priority);
    listProcess(process);

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

    setProcessState(processToKill, ZOMBIE);
    freeProcessSched(processSched);
    removeNode(sched->processes[getProcessPriority(processToKill)], processNode);


    LinkedListADT deadChildrenList = getProcessDeadChildList(processToKill);
    Node * currentNode = getFirst(deadChildrenList);
    while (currentNode != NULL) {
        Node *zombieNode = currentNode;
        currentNode = currentNode->next;
        if(zombieNode->data != NULL){
            ProcessSchedADT zombieProcessSched = zombieNode->data;
            if(zombieProcessSched->processData != NULL){
                ProcessADT zombieProcess = processSched->processData;
                freeProcess(zombieProcess);
            }
            freeProcessSched(zombieProcessSched);
        }
        freeMemory(zombieNode);
    }

    Node * parent = getProcessNode(getProcessParentPid(processToKill));
    if(parent != NULL && parent->data != NULL) {
        ProcessSchedADT parentProcessSched = (ProcessSchedADT) parent->data;
        if(parentProcessSched->processData != NULL){
            if(getProcessState(parentProcessSched->processData) != ZOMBIE){
                insert(getProcessDeadChildList(parentProcessSched->processData), processToKill);
                if(getProcessWaitingPid(parentProcessSched->processData) == getProcessPid(processToKill) && getProcessState(parentProcessSched->processData) == BLOCKED) {
                    setState(getProcessPid(parentProcessSched->processData), READY);
                }
            }
        }
    } else {
        sched->processQty--;
        freeProcess(processToKill);
    }

    if(sched->currentPid == pid){
        yield();
    }

    return SUCCESS;
}

uint16_t setState(uint32_t pid, uint64_t state) {   // siempre se le cambia a un proceso que no esta RUNNNIG (osea que pertenece a alguna lista),
                                                    // si esta RUNNING solo puede ser BLOQUEADO o puesto en READY con un yield()
                                                    // si esta BLOQUEADO lo puedo DESBLOQUEAR
                                                    // si esta READY lo PUEDO BLOQUEAR ?
                                                    // si esta ZOMBIE o quiere ir a ZOMBIE no puedo, eso solo lo hace el killProcess()
                                                    // si quiero que este RUNNING no puedo, eso solo lo puede hacer el schedule(9
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
        listProcess(processSched->processData);
        freeProcessSched(processSched);
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
    Node * child = getDeadChildNode(getProcessDeadChildList(current), getProcessPid(childProcess));
    removeNode(getProcessDeadChildList(current), child);
    sched->processQty--;
    freeProcess(childProcess);

    return toReturn;
}

static Node * getDeadChildNode(LinkedListADT list, uint32_t pid) {
    Node * processNode = NULL;
    uint8_t found = 0;

    Node * currentNode = getFirst(list);

    while(currentNode != NULL && !found){
        ProcessADT processData = (ProcessADT) currentNode->data;
        if(getProcessPid(processData) == pid){
            processNode = currentNode;
            found = 1;
        }
        currentNode =  currentNode->next;
    }
      return  processNode;
}

// FALTA ITERAR TAMBIEN SOBRE LA LISTA DE DEADCHILDREN DE CADA PROCESO
ProcessCopyListADT getProcessCopy(){
    SchedulerADT sched = getScheduler();
    ProcessCopyListADT processListCopies = allocMemory(sizeof(getProcessCopyListSize()));
    ProcessCopy * processCopyArray =  allocMemory(sched->processQty * sizeof(ProcessCopy));
    int index = 0;

    for(int i = LEVEL4; i > LEVEL0; i--) {
        Node * currentNode = getFirst(sched->processes[i]);
        while(currentNode != NULL && currentNode->data != NULL) {       // valido que el NODE no sea NULL y tampoco el ProcessSched
            ProcessSchedADT processSched = (ProcessSchedADT) currentNode->data;
            copyProcess(&processCopyArray[index], processSched->processData);
            index++;
            currentNode = currentNode->next;
        }
    }
    setProcessCopyListLength(processListCopies, index);
    setProcessCopyList(processListCopies, processCopyArray);
    return processListCopies;
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
