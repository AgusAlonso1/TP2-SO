#ifndef PROCESS_H
#define PROCESS_H 

#include <stdint.h>
#include <lib.h>
#include <globals.h>
#include <linkedListADT.h>
#include <interruptions.h>
#include <libString.h>
#include <memoryManager.h>

#define STACK_SIZE 4096


typedef struct ProcessCDT* ProcessADT;

ProcessADT createProcess(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, char immortal, char position, Function function, char **args, const int fileDescriptors[CANT_FILE_DESCRIPTORS]);
        int setProcessState(ProcessADT process, uint64_t state);
uint64_t getProcessState(ProcessADT process);
uint32_t getProcessParentPid(ProcessADT process);
uint32_t getProcessPid(ProcessADT process);
void setProcessPriority(ProcessADT process, uint32_t priority);
uint32_t getProcessPriority(ProcessADT process);
uint32_t getProcessPosition(ProcessADT process);
int freeProcess(ProcessADT process);
ProcessCopy * copyProcess(ProcessCopy * processCopy , ProcessADT process);
void setProcessStack(ProcessADT process, void * stack);
void * getProcessStack(ProcessADT process);
void setProcessReturnValue(ProcessADT process, int returnValue);
int getProcessReturnValue(ProcessADT process);
void argscopy(char*** arguments, char** args);
char getProcessMortality(ProcessADT process);
uint32_t getProcessWaitingPid(ProcessADT process);
void setProcessWaitingPid(ProcessADT process, uint32_t childPid);
void wrapper(Function function, char **args);
int getProcessReadFileDescriptor(ProcessADT process);
int getProcessWriteFileDescriptor(ProcessADT process);
int getProcessErrorFileDescriptor(ProcessADT process);


#endif
