#include <scheduler.h>
#define SCHEDULER_ADDRESS 700


/* Comentarios scheduler:
 * Me parece que mejor va a ser que el  Scheduler sea un ADT; asi queda organizado e igual (process, mm) //ver si no queda mas prolijo hacer una libreria generica de linked list
donde sea: typedef struct Node{
                            void* data;
                            struct Node *prev;
                            struct Node *next;
                          }Node;
osea, como habiamos visto en un principio con void*
 * Por ahi es mejor modularizar y hacer una biblioteca linkedList donde este el tipo Node generico y LinkedList y
ahi implementar todas las funciones basicas (add, remove, isempty, next, hastNext, etc)
 * Me parece que lo mejor es sacar lo de inicializar init aca; podemos cuando creamos el scheduler crearlo ahi a ese proceso
 * Tenemos que ver bien la logica de cambiar estados y lo de manejar procesos zombie
 * Preguntar si es mejor hacer listas con distintas prioridades o una lista y que se vayan agregando de acuerdo a la prioridad
 * En assembler hice que la interrupcion del timer, ademas de llamar al timer hanlder, llame a la funcion schedule que es
la que se enecraga de hacer el context switch de los procesos
 * Tambien estuve viendo lo de round robin, y como que vamos a tener que asignarle un quantum a cada proceso, asi que se
lo agregue al struct scheduler. La idea es que si un proceso no termina su ejecución dentro de su quantum, se pone nuevamente al final de la cola para esperar su turno.
 */


typedef struct SchedulerCDT{
    ProcessListADT processes[PRIORITY_LEVELS];
    ProcessADT currentProcess;
    uint64_t pidCounter;
    uint16_t processQuantum;
} SchedulerCDT;

void create_scheduler() {
    SchedulerADT sched = (SchedulerADT) SCHEDULER_ADDRESS;
    sched->pidCounter = 0;
}

void * schedule(void * currentStackPointer) {
    //Suponemos que no es el primer caso
    SchedulerADT sched = get_scheduler();
    set_stack(sched->currentProcess, currentStackPointer);
    uint64_t currentState = get_state(sched->currentProcess);

    if(sched->processQuantum > 0) {
        if (currentState != BLOCKED && currentState != EXITED){
            sched->processQuantum--;
            return currentStackPointer;
        }
    }

    ProcessADT processToRun;
    if(sched->processQuantum == 0 || currentState == BLOCKED || currentState == EXITED) {
        uint64_t currentPriority =  get_priority(sched->currentProcess);
        if(currentPriority != LEVEL1 && currentPriority != LEVEL4) {
            currentPriority--;
        }
        set_priority(sched->currentProcess, currentPriority); 
         
        uint8_t found = 0; 
        for(int i = LEVEL4; i > LEVEL0; i--) {
            ProcessNode * currentNode = getFirstNode(sched->processes[i]);
            while(currentNode != NULL) {
                if(get_state(currentNode->processData) == READY) {
                    if(!found) {
                        pop(currentNode);
                        processToRun = currentNode->processData;
                        found = 1;
                    } else {
                        currentNode->quantumWating++;
                        if(currentNode->quantumWating == 10) { //El quantum se puede cambiar
                            currentNode->quantumWating = 0;
                            uint64_t priority = get_priority(currentNode->processData);
                            if(priority < LEVEL3){
                                set_priority(currentNode->processData, priority);
                            }
                        }
                    }
                }
                currentNode = currentNode->next;
            }
        }
    }
    return get_stak(processToRun);
}

SchedulerADT get_scheduler() {
    return (SchedulerADT)SCHEDULER_ADDRESS;
}

void create_process_sched(char* name, char position, uint64_t priority, Function function, char **args) {
    SchedulerADT sched = get_scheduler();
    ProcessADT newProcess = create_process(sched->pidCounter, sched->pidCounter++, name, priority, READY, position, function, args);
    list_process(sched, newProcess);
}

void list_process(SchedulerADT sched, ProcessADT process) {
    add(sched->processes[get_priority(process)], process);
}
void unlist_process(SchedulerADT sched, uint64_t priority){
   sched->currentProcess = popFirst(sched->processes[priority]);
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