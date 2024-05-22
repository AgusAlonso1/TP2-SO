#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <linkedList.h>
#include <stddef.h>
#include <definitions.h>

#define PRIORITY_LEVELS 5
#define SUCCESS 0
#define ERROR -1

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

void createScheduler();                                                         //create scheduler
SchedulerADT getScheduler();
void * schedule(void * currentStackPointer);                                    //schedule process ---> cambia de proceso running(se ejecuta cuando timer tick)
void createProcessSched(char* name, char position, uint64_t priority, Function function, char **args);                        //create process ----> se crea un proceso
void listProcess(SchedulerADT sched, ProcessADT process);
void unlistProcess(SchedulerADT *sched, uint64_t priority);
void waitProcessPid(uint32_t pid, uint64_t state);                              //waitpid -----> block al padre y vemos si bajamos prioridad
void setPriority(ProcessADT process, uint64_t priority);                        //cambiar prioridad de un proceso
void yield();                                                                   //yield -----> renunciar al CPU (setear el quantum del proceso en 0 y forzar un timer tick)
void killProcess(uint32_t pid);                                                 //kill ------> mata al proceso
ProcessNode * getProcessNode(uint32_t pid);
uint16_t setState(uint32_t pid, uint64_t state);                                //cambiar estado ----> cambia el estado del proceso
void exitProcess(int ret);                                                      //exit de la wrapper de proceso (maneja la terminacion de un proceso)
uint32_t getPid();                                                              //getpid -----> usa getpid del process
uint32_t getParentPid();                                                        //getppid ------> usa getppid del process
ProcessListADT getProcessCopy();


//ps -----> hacer copia de la process info en el momento ese



#endif