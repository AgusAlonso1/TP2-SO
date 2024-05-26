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
    ProcessListADT processes[PRIORITY_LEVELS]; 
    ProcessADT currentProcess;
    uint32_t pidCounter;
    uint16_t processQuantum;
    uint64_t processQty;
} SchedulerCDT;

void createScheduler() {
    SchedulerADT sched = (SchedulerADT) SCHEDULER_ADDRESS;
    sched->pidCounter = 0;
    sched->processQty = 0;
}

void * schedule(void * currentStackPointer) {
    //Suponemos que no es el primer caso
    SchedulerADT sched = getScheduler();
    setProcessStack(sched->currentProcess, currentStackPointer);
    uint64_t currentState = getProcessState(sched->currentProcess);

    if(sched->processQuantum > 0) {
        if (currentState != BLOCKED && currentState != ZOMBIE){
            sched->processQuantum--;
            return currentStackPointer;
        }
    }

    ProcessADT processToRun;
    if(sched->processQuantum == 0 || currentState == BLOCKED || currentState == ZOMBIE) {
        uint64_t currentPriority =  getProcessPriority(sched->currentProcess);
        if(currentPriority != LEVEL1 && currentPriority != LEVEL4) {
            currentPriority--;
        }
        setPriority(sched->currentProcess, currentPriority);

        uint8_t found = 0; 
        for(int i = LEVEL4; i > LEVEL0; i--) {
            ProcessNode * currentNode = getFirstNode(sched->processes[i]);
            while(currentNode != NULL) {
                if(getProcessState(currentNode->processData) == READY) {
                    if(!found) {
                        processToRun = currentNode->processData;
                        pop(sched->processes[i], currentNode);
                        found = 1;
                    } else {
                        currentNode->quantumWating++;
                        if(currentNode->quantumWating == 10) { //El quantum se puede cambiar
                            currentNode->quantumWating = 0;
                            uint64_t priority = getProcessPriority(currentNode->processData);
                            if(priority < LEVEL3){
                                setPriority(currentNode->processData, priority+1);
                                pop(sched->processes[i], currentNode);
                            }
                        }
                    }
                }
                currentNode = currentNode->next;
            }
        }
    }
    return getProcessStack(processToRun);
}

SchedulerADT getScheduler() {
    return (SchedulerADT) SCHEDULER_ADDRESS;
}

void createProcessSched(char* name, char position, uint64_t priority, Function function, char **args) {
    SchedulerADT sched = getScheduler();
    int currentPid = sched->pidCounter; //Sino hacemos esto tira un warning raro
    sched->processQty++;
    ProcessADT newProcess = createProcess(currentPid, sched->pidCounter++, name, priority, READY, position, function, args);
    listProcess(sched, newProcess);
}

void listProcess(SchedulerADT sched, ProcessADT process) {
    add(sched->processes[getProcessPriority(process)], process);
}

void unlistFirtsProcess(SchedulerADT sched, uint64_t priority){
   sched->currentProcess = popFirst(sched->processes[priority]);
}

//le cambia la prioridad al proceso y lo cambia de lista (el proceso que toma como argumento ya esta popeado)
void setPriority(ProcessADT process, uint64_t priority){
    SchedulerADT sched = getScheduler();
    setProcessPriority(sched->currentProcess, priority);
    add(sched->processes[priority], process);
}

void yield(){
    SchedulerADT sched = getScheduler();
    sched->processQuantum = 0;
    forceTimerTick();
}

void killProcess(uint32_t pid){
    SchedulerADT sched = getScheduler();
    ProcessNode * processNode = getProcessNode(pid);

    char isProcessRunning = getProcessPid(sched->currentProcess) == pid ? 1 : 0;    //como sacamos siempre el nodo y lo eliminaos,
                                                                                            // el caso en el que quiera matar el que esta RUNNNING,
                                                                                            // hay que ver si el pid que pide el igual al pid del currentProcess
    if(processNode == NULL && !isProcessRunning) {
        return;
    }

    ProcessADT processToKill;
    if(!isProcessRunning){
        processToKill = processNode->processData;
        pop(sched->processes[getProcessPriority(processToKill)], processNode);       //lo elimino de donde sea que este
    } else{
        processToKill = sched->currentProcess;
    }

    if(getProcessState(processToKill) == ZOMBIE || getProcessMortality(processToKill)) {
        return;
    }
    
    setProcessState(processToKill, ZOMBIE);

    add(sched->processes[LEVEL0], processToKill);

    LinkedListADT deadChildrenList = getProcessDeadChildList(processToKill);
    Node *currentNode = deadChildrenList->first;

    while (currentNode != NULL) {
        Node *zombieNode = currentNode;
        currentNode = currentNode->next;
        sched->processQty--;
        freeProcess(zombieNode->data);
        freeMemory(zombieNode);
    }

    ProcessNode * parent = getProcessNode(getParentPid(processToKill));
    if(parent != NULL && getProcessState(parent->processData) != ZOMBIE) {
        insert(getProcessDeadChildList(parent->processData), processToKill)
        if(getProcessWatingPid(parent->processData) == getProcessPid(processToKill) && getProcessState(parent->processData) == BLOCKED) {
            setState(parent, READY);
        }
    } else {
        sched->processQty--;
        freeProcess(processToKill);
    }

   if(getProcessPid(sched->currentProcess) == getProcessPid(processToKill)){
       yield();
   }
}

//setea el estado de un proceso (identf con su id)
uint16_t setState(uint32_t pid, uint64_t state){
    SchedulerADT sched = getScheduler();
    ProcessNode * processNode = getProcessNode(pid);

    //si el nodo es nulo no existe un proceso con ese pid
    if(processNode == NULL){
        return ERROR;
    }

    uint64_t currentState = getProcessState(processNode->processData);

    //el estado RUNNING solo se cambia en schedule el estado ZOMBIE solo se cambia en exit/killProcess; el tema es ver donde hacemos lo de ZOMBIE
    if(currentState == RUNNING || currentState == ZOMBIE || state == ZOMBIE){
        return ERROR;
    } else if(currentState == state){
        return SUCCESS;
    }

    setProcessState(processNode->processData, state);      //se cambia el estado

    if(state == BLOCKED){                           //si el nuevo estado es bloqueado, va al maximo nivel, sino queda donde esta
        add(sched->processes[LEVEL4], processNode->processData);
        pop(sched->processes[LEVEL4],processNode);
    }

    return SUCCESS;
}

ProcessNode * getProcessNode(uint32_t pid){
    SchedulerADT sched = getScheduler();
    ProcessNode* processNode = NULL;
    uint8_t found = 0;
    for(int i = LEVEL4; i > LEVEL0; i--) {
        ProcessNode * currentNode = getFirstNode(sched->processes[i]);
        while(currentNode != NULL && !found) {
            if(getProcessPid(currentNode->processData) == pid) {
                processNode = currentNode;
                found = 1;
            }
            currentNode = currentNode->next;
        }
    }
    return processNode;
}

uint32_t getPid(){
    SchedulerADT sched = getScheduler();
    return getProcessPid(sched->currentProcess);
}

uint32_t getParentPid(){
    SchedulerADT sched = getScheduler();
    return getProcessParentPid(sched->currentProcess);
}


void exitProcess(int returnValue){
    SchedulerADT sched = getScheduler();
    setProcessReturnValue(sched->currentProcess, returnValue);
    killProcess(getProcessPid(sched->currentProcess));
}

uint64_t wait_process_pid(uint32_t pid) {
    SchedulerADT sched = getScheduler();
    ProcessNode * childNode =  getProcessNode(pid);
    if(childNode == NULL) {
        return ERROR;
    }
    ProcessADT childProcess = childNode->processData;
    uint32_t parentPid = getProcessParentPid(childProcess);
    if(parentPid != getProcessPid(sched->currentProcess)) {
        return ERROR;
    }
    setProcessWatingPid(sched->currentProcess, pid);
    if(getProcessState(childProcess) != ZOMBIE) {
        setState(sched->currentProcess, BLOCKED);
        yield();
    } 
    uint64_t toReturn = getProcessReturnValue(childProcess);
    removeNode(getProcessDeadChildList(sched->currentProcess), childNode);
    sched->processQty--;
    freeProcess(childProcess);
    return toReturn;
}


ProcessCopyListADT getProcessCopy(){
    SchedulerADT sched = getScheduler();
    ProcessCopyListADT processListCopies = allocMemory(sizeof (ProcessCopyListCDT));
    ProcessCopy * processCopyArray =  allocMemory(scheduler->qtyProcesses * sizeof(ProcessCopy));
    int index = 0;

    ProcessADT copy;
    for(int i = LEVEL4; i > LEVEL0; i--) {
        ProcessNode * currentNode = getFirstNode(sched->processes[i]);
        while(currentNode != NULL) {
            copyProcess(&processCopyArray[index], currentNode->processData)
            index++;
            currentNode = currentNode->next;
        }
    }
    setProcessCopyListLength(processListCopies, index);
    setProcessCopyList(processListCopies, processCopyArray);
    return processListCopies;
}


