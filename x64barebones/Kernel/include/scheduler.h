#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <linkedList.h>

#define PRIORITY_LEVELS 5

enum quantum_level {
    QL1 = 2,
    QL2 = 3,
    QL3 = 4,
    QL4 = 5
};

enum levels {
    LEVEL0 = 0,
    LEVEL1 = 1,
    LEVEL2 = 2,
    LEVEL3 = 3,
    LEVEL4 = 4
};

typedef struct SchedulerCDT* SchedulerADT;

void create_scheduler();
void * schedule(void * currentStackPointer);
void create_process_sched();
void list_process(SchedulerADT sched, ProcessADT process);
void unlist_process(SchedulerADT *sched, uint64_t priority);
void* schedule(void* prevProcessStack);
void wait_process_pid(uint32_t pid, uint64_t state);

//create scheduler
//create process ----> se crea un proceso
    //add process to list -----> agregamos el proceso que se creo al array de procesos
//schedule process ---> cambia de proceso running(se ejecuta cuando timer tick)
//cambiar estado ----> cambia el estado del proceso
//kill ------> mata al proceso
    //delete process from list -----> elimina proceso del la lista
//waitpid -----> block al padre y vemos si bajamos prioridad
//cambiar prioridad de un proceso
//yield -----> renunciar al CPU (setear el quantum del proceso en 0 y forzar un timer tick)
//ps -----> hacer copia de la process info en el momento ese
//getpid -----> usa getpid del process
//getppid ------> usa getppid del process
//time running ----> asignamiento del tiempo


#endif