#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>
#include <definitions.h>
#include <processes.h>
#include <memoryManager.h>
#include <pipeMaster.h>

typedef struct ProcessSchedCDT * ProcessSchedADT;
typedef struct SchedulerCDT* SchedulerADT;

void createScheduler();
SchedulerADT getScheduler();
void * schedule(void * currentStackPointer);
int32_t createProcessFromSched(char* name, char position, uint64_t priority, Function function, char **args, uint32_t parentPid, char mortality, const int fileDescriptors[CANT_FILE_DESCRIPTORS]);                //create process ----> se crea un proceso
void listProcess(ProcessSchedADT processSched);
int waitProcessPid(uint32_t pid);
int64_t setPriority(uint32_t pid, uint64_t priority);
void yield();
int64_t killProcess(uint32_t pid);
Node * getProcessNode(uint32_t pid);
int16_t setState(uint32_t pid, uint64_t state);
void exitProcess(int ret);
uint32_t getCurrentPid();
uint32_t getCurrentParentPid();
ProcessCopyList * getProcessCopy();
ProcessSchedADT createProcessSched(ProcessADT process);
void freeProcessSched(ProcessSchedADT processSched);
ProcessADT getCurrentProcess();
void removeFromAllProcesses(uint32_t pid);
void freeProcessCopy(ProcessCopyList * processCopyList);
int64_t block(uint32_t pid);
int64_t isProcessAlive(uint32_t pid);
int getCurrentReadFileDescriptor();
int getCurrentWriteFileDescriptor();
int getCurrentErrorFileDescriptor();
void killForegroundProcess();






#endif