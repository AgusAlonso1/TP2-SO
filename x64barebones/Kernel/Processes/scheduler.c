#include <scheduler.h>

/* Comentarios scheduler:
 * Cree una libreria generica LinkedListADT, que si modificamos unas cosas podemos usarla en vez de la que tenemos ahora
lo hice porque por un tema de includes, no podia tener una lista del tipo ProcessListADT en processes.c, entonces tiraba
error en la parte de getProcessDeadChildList
 * Le agregue al scheduler cantidad de procesos, para que a la hora de hacer ps (copia de procesos) sea mas facil pasarselo
al userland como un array que como una lista
 * Cree en globals un tipo processCopy, para que asi cuadno se lo pasemos a userland esta info pueda ser manipulada. De otra forma
se nos es imposible acceder a la info del struct
 * En globals puse la definicion del tipo Function, mismo puse las funciones esas que tienen q ver con string que se usan
tanto en kernel coo userland; tambn le agregue las definiciones los tipos ZOMBIE, READY, RUNNING, BLOCKED ahi
 */

// TAREAS IMPORTANTES PENDIENTES
//ver logica de backgournd y foreground
//revisar primer caso schedule

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

static int quantumLevel[5] = {0, 2, 3, 4, 5};
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
        uint64_t currentPriority;
        if(sched->currentProcess != NULL) {
            if (sched->killCurrentProcess && !getProcessMortality(sched->currentProcess)) {
                killProcess(getProcessPid(sched->currentProcess));
            } else {
                currentPriority  = getProcessPriority(sched->currentProcess);
                if (currentPriority != LEVEL1 && currentPriority != LEVEL4) {
                    currentPriority--;
                }
            }
        }
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
        if(sched->currentProcess != NULL){
            setPriority(sched->currentProcess, currentPriority);
        }
        sched->currentProcess = processToRun;
        sched->processQuantum = quantumLevel[getProcessPriority(processToRun)];
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


