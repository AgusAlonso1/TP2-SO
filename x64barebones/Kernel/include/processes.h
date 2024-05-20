#ifndef PROCESS_H
#define PROCESS_H 

#include <stdint.h>
#include <base_memory_manager.h>
#include <lib.h>

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define EXITED 3
#define FOREGROUND 0
#define BACKGROUND 1
#define STACK_SIZE 4096


typedef int (*Function)(int, char **);
typedef void (*wrp)(Function, char **);


typedef struct ProcessCDT* ProcessADT;

ProcessADT createProcess(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, uint64_t state, char position, Function function, char **args);
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
ProcessADT copyProcess(ProcessADT process, Function function, char ** args);
void setProcessStack(ProcessADT process, void * stack);
void * getProcessStack(ProcessADT process);
void argscopy(char** arguments, char** args);


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
