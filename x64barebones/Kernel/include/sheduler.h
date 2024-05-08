#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <processes.h>

typedef SchedulerCDT * SchedulerADT;

void create_scheduler();
void create_process_sched();
void list_process(SchedulerADT sched, ProcessADT process);
void unlist_process(SchedulerADT *sched, ProcessNode *nodeToRemove);
void* schedule(void* prevProcessStack);

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