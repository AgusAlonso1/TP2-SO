#ifndef PROCESS_H
#define PROCESS_H 

#include <stdint.h>
#include <memoryManager.h>
#include <lib.h>
#include <globals.h>
#include <linkedListADT.h>

#define FOREGROUND 0
#define BACKGROUND 1
#define STACK_SIZE 4096



typedef void (*wrp)(Function, char **);


typedef struct ProcessCDT* ProcessADT;

ProcessADT createProcess(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, char inmortal, char position, Function function, char **args);
void setProcessState(ProcessADT process, uint64_t state);
uint64_t getProcessState(ProcessADT process);
void setProcessParentPid(ProcessADT process, uint32_t parentPid);
uint32_t getProcessParentPid(ProcessADT process);
void setProcessPid(ProcessADT process, uint32_t pid);
uint32_t getProcessPid(ProcessADT process);
void setProcessPriority(ProcessADT process, uint32_t priority);
uint32_t getProcessPriority(ProcessADT process);
void setProcessPosition(ProcessADT process, uint32_t position);
uint32_t getProcessPosition(ProcessADT process);
void freeProcess(ProcessADT process);
ProcessCopy * copyProcess(ProcessCopy * processCopy , ProcessADT process);
void setProcessStack(ProcessADT process, void * stack);
void * getProcessStack(ProcessADT process);
void setProcessReturnValue(ProcessADT process, int returnValue);
uint64_t getProcessReturnValue(ProcessADT process);
void argscopy(char** arguments, char** args);
int getProcessMortality(ProcessADT process);
LinkedListADT getProcessDeadChildList(ProcessADT process);
uint32_t getProcessWatingPid(ProcessADT process);
void setProcessWatingPid(ProcessADT process, uint32_t childPid); 


#endif



// Priority-based round Robin.
// El sistema también DEBERÁ proveer los siguientes servicios:
// ● Crear y finalizar un proceso. DEBERÁ soportar el pasaje de parámetros.
// ● Obtener el ID del proceso que llama.
// ● Listar todos los procesos: nombre, ID, prioridad, stack y base pointer, foreground y
// cualquier otra variable que consideren necesaria.
// ● Matar un proceso arbitrario.
// ● Modificar la prioridad de un proceso arbitrario.
// ● Bloquear y desbloquear un proceso arbitrario.
// ● Renunciar al CPU
// ● Esperar a que los hijos terminen
