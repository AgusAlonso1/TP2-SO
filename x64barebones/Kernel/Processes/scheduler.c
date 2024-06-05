#include <scheduler.h>

/* Comentarios scheduler:
 *
 */

// TAREAS IMPORTANTES PENDIENTES
//ver logica de backgournd y foreground

typedef struct SchedulerCDT{
    LinkedListADT processes[PRIORITY_LEVELS];
    ProcessADT currentProcess;
    uint32_t pidCounter;
    uint16_t processQuantum;
    uint64_t processQty;
    char killCurrentProcess;
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
    sched->pidCounter = 0;
    sched->processQty = 0;
    sched->killCurrentProcess = 0;
    sched->processQuantum = 0;
}


void * schedule(void * currentStackPointer) {
    SchedulerADT sched = getScheduler();

    setProcessStack(sched->currentProcess, currentStackPointer);
    uint64_t currentState = getProcessState(sched->currentProcess);

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
                    if (getProcessState(processSched->processData) == READY) {
                        if (!found) {
                            processToRun = processSched->processData;
                            freeProcessSched(processSched);
                            removeNode(sched->processes[i], currentNode);
                            found = 1;
                        } else {
                            processSched->quantumWaiting++;
                            if (processSched->quantumWaiting == 10) { //El quantum se puede cambiar
                                processSched->quantumWaiting = 0;
                                uint64_t priority = getProcessPriority(processSched->processData);
                                if (priority < LEVEL3) {
                                    setPriority(processSched->processData, priority + 1);
                                    freeProcessSched(processSched);
                                    removeNode(sched->processes[i], currentNode);
                                }
                            }
                        }
                    }
                    currentNode = currentNode->next;
                }
            }
        }

        if(sched->currentProcess != NULL) {
            uint64_t currentPriority  = getProcessPriority(sched->currentProcess);
            if (sched->killCurrentProcess && !getProcessMortality(sched->currentProcess)) {
                killProcess(getProcessPid(sched->currentProcess));
                sched->killCurrentProcess = 0;
            } else {
                if(currentPriority != LEVEL1 && currentPriority != LEVEL4) {
                    currentPriority--;
                }
                setPriority(sched->currentProcess, currentPriority);
                setProcessState(sched->currentProcess, READY);
            }
        }


        sched->currentProcess = processToRun;
        sched->processQuantum = quantumLevel[getProcessPriority(processToRun)];
        setProcessState(sched->currentProcess, RUNNING);
    }

    return getProcessStack(processToRun);
}

SchedulerADT getScheduler() {
    return (SchedulerADT) SCHEDULER_ADDRESS;
}

void createProcessFromSched(char* name, char position, uint64_t priority, Function function, char **args, uint32_t parentPid) {
    SchedulerADT sched = getScheduler();
    uint32_t  currentPid = sched->pidCounter;
    ProcessADT newProcess = NULL;

    newProcess = createProcess(parentPid, sched->pidCounter++, name, priority, READY, position, function, args);

    listProcess(newProcess);
    sched->processQty++;

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

/*
void unListFirstProcess(SchedulerADT sched, uint64_t priority){
   sched->currentProcess = remove(sched->processes[priority]);
}
*/

//le cambia la prioridad al proceso y lo cambia de lista (el proceso que toma como argumento ya esta popeado)
void setPriority(ProcessADT process, uint64_t priority){
    setProcessPriority(process, priority);
    listProcess(process);
}

void yield(){
    SchedulerADT sched = getScheduler();
    sched->processQuantum = 0;
    forceTimerTick();
}

void killProcess(uint32_t pid){
    SchedulerADT sched = getScheduler();
    Node * processNode = getProcessNode(pid);

    char isProcessRunning = getProcessPid(sched->currentProcess) == pid ? 1 : 0;    //como sacamos siempre el nodo y lo eliminaos,
                                                                                            // el caso en el que quiera matar el que esta RUNNNING,// hay que ver si el pid que pide el igual al pid del currentProcess
    if(processNode == NULL && !isProcessRunning) {
        return;
    }

    ProcessADT processToKill = NULL;
    if(!isProcessRunning){
        ProcessSchedADT processSched = (ProcessSchedADT) processNode->data;
        processToKill = processSched->processData;
        if(processToKill == NULL){
            return;
        }
        freeProcessSched(processSched);
        removeNode(sched->processes[getProcessPriority(processToKill)], processNode);       //lo elimino de donde sea que este

        if(getProcessState(processToKill) == ZOMBIE || getProcessMortality(processToKill)) {
            return;
        }

        setProcessState(processToKill, ZOMBIE);
        insert(sched->processes[LEVEL0], createProcessSched(processToKill));

        // Manejo de la lista de ZOMBIES en el padre; aca insertamos ProcessADT como data en NODE, no como en le resto que insertamos ProcessSched como data en NODE
        LinkedListADT deadChildrenList = getProcessDeadChildList(processToKill);
        Node * currentNode = getFirst(deadChildrenList);

        while (currentNode != NULL) {
            Node *zombieNode = currentNode;
            currentNode = currentNode->next;
            sched->processQty--;
            freeProcess(zombieNode->data);
            freeMemory(zombieNode);
        }

        Node * parent = getProcessNode(getProcessParentPid(processToKill));

        if(parent != NULL ) {
            ProcessSchedADT parentProcessSched = (ProcessSchedADT) parent->data;
            if(getProcessState(parentProcessSched->processData) != ZOMBIE){
                insert(getProcessDeadChildList(parentProcessSched->processData), processToKill);
                if(getProcessWaitingPid(parentProcessSched->processData) == getProcessPid(processToKill) && getProcessState(parentProcessSched->processData) == BLOCKED) {
                    setState(getProcessPid(parentProcessSched->processData), READY);
                }
            }

        } else {
            sched->processQty--;
            freeProcess(processToKill);
        }


    } else {
        sched->killCurrentProcess = 1;
        yield();
    }
}

//setea el estado de un proceso (identf con su id)
uint16_t setState(uint32_t pid, uint64_t state){
    SchedulerADT sched = getScheduler();
    Node * processNode = getProcessNode(pid);

    //si el nodo es nulo no existe un proceso con ese pid
    if(processNode == NULL){
        return ERROR;
    }

    ProcessSchedADT processSched = (ProcessSchedADT) processNode->data;
    if(processSched == NULL){
        return ERROR;
    }

    uint64_t currentState = getProcessState(processSched->processData);

    //el estado RUNNING solo se cambia en schedule el estado ZOMBIE solo se cambia en exit/killProcess; el tema es ver donde hacemos lo de ZOMBIE
    if(currentState == RUNNING || currentState == ZOMBIE || state == ZOMBIE){
        return ERROR;
    } else if(currentState == state){
        return SUCCESS;
    }

    setProcessState(processSched->processData, state);      //se cambia el estado

    if(state == BLOCKED){                           //si el nuevo estado es bloqueado, va al maximo nivel, sino queda donde esta
        insert(sched->processes[LEVEL4], processSched->processData);
        freeProcessSched(processSched);
        removeNode(sched->processes[LEVEL4],processNode);
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
    return getProcessPid(sched->currentProcess);
}

uint32_t getCurrentParentPid(){
    SchedulerADT sched = getScheduler();
    return getProcessParentPid(sched->currentProcess);
}


void exitProcess(int returnValue){
    SchedulerADT sched = getScheduler();
    setProcessReturnValue(sched->currentProcess, returnValue);
    killProcess(getProcessPid(sched->currentProcess));
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
    setProcessWaitingPid(sched->currentProcess, pid);
    if(getProcessState(childProcess) != ZOMBIE) {
        setState(getCurrentPid(), BLOCKED);
        yield();
    }
    uint64_t toReturn = getProcessReturnValue(childProcess);
    Node * child = getDeadChildNode(getProcessDeadChildList(sched->currentProcess), getProcessPid(childProcess));
    removeNode(getProcessDeadChildList(sched->currentProcess), child);
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

void setPrioritySyscall(uint32_t pid, uint64_t priority){
    SchedulerADT sched = getScheduler();
    if(sched->currentProcess != NULL && getProcessPid(sched->currentProcess) == pid){
        if(getProcessPriority(sched->currentProcess) == priority){
            return;
        }
        setProcessPriority(sched->currentProcess, priority);
        yield();
    }

    Node * processNode = getProcessNode(pid);
    if(processNode == NULL){
        return;
    }

    ProcessSchedADT processSched = (ProcessSchedADT) processNode->data;
    if(processSched == NULL){
        return;
    }

    ProcessADT process = processSched->processData;
    if(process == NULL){
        return;
    }
    uint64_t processPriority = getProcessPriority(process);

    if(processPriority == priority){
        return;
    }

    freeMemory(processSched);
    removeNode(sched->processes[getProcessPriority(process)], processNode);

    setPriority(process, priority);
}
