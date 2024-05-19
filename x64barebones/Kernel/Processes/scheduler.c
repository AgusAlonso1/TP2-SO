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
    SchedulerADT sched = get_scheduler();
    setStack(sched->currentProcess, currentStackPointer);
    uint64_t currentState = getState(sched->currentProcess);

    if(sched->processQuantum > 0) {
        if (currentState != BLOCKED && currentState != EXITED){
            sched->processQuantum--;
            return currentStackPointer;
        }
    }

    ProcessADT processToRun;
    if(sched->processQuantum == 0 || currentState == BLOCKED || currentState == EXITED) {
        uint64_t currentPriority =  getPriority(sched->currentProcess);
        if(currentPriority != LEVEL1 && currentPriority != LEVEL4) {
            currentPriority--;
        }
        setPriority(sched->currentProcess, currentPriority, sched);

        uint8_t found = 0; 
        for(int i = LEVEL4; i > LEVEL0; i--) {
            ProcessNode * currentNode = getFirstNode(sched->processes[i]);
            while(currentNode != NULL) {
                if(getState(currentNode->processData) == READY) {
                    if(!found) {
                        pop(currentNode); //no entiendo porque pop. Cuando va el popFirst? Que pasa con la memoria del nodo?
                        processToRun = currentNode->processData;
                        found = 1;
                    } else {
                        currentNode->quantumWaiting++;
                        if(currentNode->quantumWaiting == 10) { //El quantum se puede cambiar
                            currentNode->quantumWaiting = 0;
                            uint64_t priority = getPriority(currentNode->processData);
                            if(priority < LEVEL3){
                                pop(currentNode);     //mismo aca, la use porque supongo me elimina el nodo de la lista pero no la info,
                                                            //pero sigo sin entender que pasa con la memoria del nodo
                                setPriority(currentNode->processData, priority+1, sched);
                            }
                        }
                    }
                }
                currentNode = currentNode->next;
            }
        }
    }
    return getStack(processToRun);
}

SchedulerADT getScheduler() {
    return (SchedulerADT) SCHEDULER_ADDRESS;
}

void createProcessSched(char* name, char position, uint64_t priority, Function function, char **args) {
    SchedulerADT sched = get_scheduler();
    ProcessADT newProcess = createProcess(sched->pidCounter, sched->pidCounter++, name, priority, READY, position, function, args);
    listProcess(sched, newProcess);
}

void listProcess(SchedulerADT sched, ProcessADT process) {
    add(sched->processes[getPriority(process)], process);
}

void unlistFirtsProcess(SchedulerADT sched, uint64_t priority){
   sched->currentProcess = popFirst(sched->processes[priority]);
}

//le cambia la prioridad al proceso y lo cambia de lista
void setPriority(ProcessADT process, uint64_t priority, SchedulerADT sched){
    setProcessPriority(sched->currentProcess, currentPriority);
    add(sched->processes[priority], process);
}

void yield(){
    SchedulerADT sched = getScheduler();
    sched->processQuantum = 0;
    forceTimerTick();
}

void killProcess(){

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