#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>
#include <definitions.h>
#include <processes.h>
//#include <memoryasm.h>
#include <memoryManager.h>

#define PRIORITY_LEVELS 5
#define SUCCESS 0
#define ERROR (-1)
#define IDLE 0
#define SHELL 1

enum levels {
    LEVEL0 = 0,
    LEVEL1 = 1,
    LEVEL2 = 2,
    LEVEL3 = 3,
    LEVEL4 = 4
};


typedef struct ProcessSchedCDT * ProcessSchedADT;
typedef struct SchedulerCDT* SchedulerADT;

void createScheduler();                                                         //create scheduler
SchedulerADT getScheduler();
void * schedule(void * currentStackPointer);                                    //schedule process ---> cambia de proceso running(se ejecuta cuando timer tick)
uint32_t createProcessFromSched(char* name, char position, uint64_t priority, Function function, char **args, uint32_t parentPid);                     //create process ----> se crea un proceso
void listProcess(ProcessADT process);
void unListFirstProcess(SchedulerADT sched, uint64_t priority);
uint64_t waitProcessPid(uint32_t pid);                           //waitpid -----> block al padre y vemos si bajamos prioridad
uint64_t setPriority(uint32_t pid, uint64_t priority);                       //cambiar prioridad de un proceso
void yield();                                                                   //yield -----> renunciar al CPU (setear el quantum del proceso en 0 y forzar un timer tick)
uint64_t killProcess(uint32_t pid);                                                //kill ------> mata al proceso
Node * getProcessNode(uint32_t pid);
uint16_t setState(uint32_t pid, uint64_t state);                                //cambiar estado ----> cambia el estado del proceso
void exitProcess(int ret);                                                      //exit de la wrapper de proceso (maneja la terminacion de un proceso)
uint32_t getCurrentPid();                                                              //getpid -----> usa getpid del process
uint32_t getCurrentParentPid();                                                     //getppid ------> usa getppid del process
ProcessCopyList * getProcessCopy();                                           //ps -----> hacer copia de la process info en el momento ese
void exitProcess(int returnValue);
ProcessSchedADT createProcessSched(ProcessADT process);
void freeProcessSched(ProcessSchedADT processSched);
ProcessADT getCurrentProcess();
void removeFromAllProcesses(uint32_t pid);





#endif