#include <scheduler.h>
#define SCHEDULER_ADDRESS 700


/* Comentarios scheduler:
 * Cambie todos los nombres de las funciones para que queden camel case, porque en algunos archivos estaba snake y en otros camel
 * No entiendo porque hacemos pop y no liberamos la memoria del nodo, como que perdemos el nodo
 * Tenemos que ver bien la logica de cambiar estados y lo de manejar procesos zombie
 * En assembler (libasm) cree la interrupcion que fuerza la interrupcion del timer forceTimerTick
 * Hice que lo de cambiar prioridad no solo cambie el valor en el ProcessADT, sino que tambien lo cambie de lista
(estaria genial si alguno lo quiere revisar)
 * Hice la funcion yield(), que basicamente le cede el CPU a algun otro proceso
 * Tenemos que ver bien que onda el tema del primer proceso. Como seria la logica?
 * Dudas con los estados. EXITED es lo mismo que ZOMBIE? lo mismo que DEAD? como seria?
 */


typedef struct SchedulerCDT{
    ProcessListADT processes[PRIORITY_LEVELS];
    ProcessADT currentProcess;
    uint32_t pidCounter;
    uint16_t processQuantum;
} SchedulerCDT;

void createScheduler() {
    SchedulerADT sched = (SchedulerADT) SCHEDULER_ADDRESS;
    sched->pidCounter = 0;
}

void * schedule(void * currentStackPointer) {
    //Suponemos que no es el primer caso
    SchedulerADT sched = getScheduler();
    setProcessStack(sched->currentProcess, currentStackPointer);
    uint64_t currentState = getProcessState(sched->currentProcess);

    if(sched->processQuantum > 0) {
        if (currentState != BLOCKED && currentState != EXITED){
            sched->processQuantum--;
            return currentStackPointer;
        }
    }

    ProcessADT processToRun;
    if(sched->processQuantum == 0 || currentState == BLOCKED || currentState == EXITED) {
        uint64_t currentPriority =  getProcessPriority(sched->currentProcess);
        if(currentPriority != LEVEL1 && currentPriority != LEVEL4) {
            currentPriority--;
        }
        setPriority(sched->currentProcess, currentPriority, sched);

        uint8_t found = 0; 
        for(int i = LEVEL4; i > LEVEL0; i--) {
            ProcessNode * currentNode = getFirstNode(sched->processes[i]);
            while(currentNode != NULL) {
                if(getProcessState(currentNode->processData) == READY) {
                    if(!found) {
                        processToRun = currentNode->processData;
                        pop(currentNode);
                        found = 1;
                    } else {
                        currentNode->quantumWating++;
                        if(currentNode->quantumWating == 10) { //El quantum se puede cambiar
                            currentNode->quantumWating = 0;
                            uint64_t priority = getProcessPriority(currentNode->processData);
                            if(priority < LEVEL3){
                                setPriority(currentNode->processData, priority+1, sched);
                                pop(currentNode);
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
void setPriority(ProcessADT process, uint64_t priority, SchedulerADT sched){
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
    if(processNode == NULL && !isProcessRunning){
        return;
    }

    ProcessADT processToKill;
    if(!isProcessRunning){
        processToKill = processNode->processData;
        pop(processNode);       //lo elimino de donde sea que este
    } else{
        processToKill = sched->currentProcess;
    }

    if(getProcessState(processToKill) == EXITED){   //no se si va EXITED O ZOMBIE
        return;
    }

    setProcessState(processToKill, EXITED);
    add(sched->processes[LEVEL0], processToKill);   //agregamos al proceso a la lista de EXITED/muertos?

    //veo si tiene zombies
    //lo meto como zombie en su padre
        //veo si es el que le falta a su padre para terminar

   if(getProcessPid(sched->currentProcess) == getProcessPid(processToKill)){
       yield();
   }
}

//setea el estado de un proceso (identf con su id)
uint16_t setState(uint32_t pid, uint64_t state){
    SchedulerADT sched = getScheduler();
    ProcessNode * processNode = getProcessNode(pid);

    //si el nodo en nulo no existe un proceso con ese pid
    if(processNode == NULL){
        return ERROR;
    }

    uint64_t currentState = getProcessState(processNode->processData);

    //el estado RUNNING solo se cambia en schedule el estado EXITED solo se cambia en exit/killProcess; el tema es ver donde hacemos lo de ZOMBIE
    //EXITED es lo mismo que ZOMBIE? lo mismo que DEAD? como seria?
    if(currentState == RUNNING || currentState == EXITED || state == EXITED){
        return ERROR;
    }else if(currentState == state){
        return SUCCESS;
    }

    setProcessState(processNode->processData, state);      //se cambia el estado

    if(state == BLOCKED){                           //si el nuevo estado es bloqueado, va al maximo nivel, sino queda donde esta
        add(sched->processes[LEVEL4], processNode->processData);
        pop(processNode);
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

void exitProcess(int ret){

}

// void wait_process_pid(uint32_t pid, uint64_t *status){
//     SchedulerADT sched = get_scheduler();

//     for (int i = 0; i < PRIORITY_LEVELS; i++) {
//         ProcessNode *current = sched->processes[i];
//         while (current != NULL) {
//             if (current->processData->pid == pid) {
//                 //
//             }
//         }
//     }
// }