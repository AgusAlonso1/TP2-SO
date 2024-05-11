#ifndef PROCESS_H
#define PROCESS_H 

#include <stdint.h>
#include <base_memory_manager.h>
#include <lib.h>

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define FOREGROUND 0
#define BACKGROUND 1
#define STACK_SIZE 4096


typedef int (*Function)(int argc, char **args);


typedef struct ProcessCDT* ProcessADT;

ProcessADT create_process(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, uint64_t state, char position, Function function, char **args);
void set_state(ProcessADT process, uint64_t state);
uint64_t get_state(ProcessADT process);
uint64_t kill_process();
void set_parentPid(ProcessADT process, uint32_t parentPid);
uint32_t get_parentPid(ProcessADT process);
void set_priority(ProcessADT process, uint32_t priority);
uint32_t get_priority(ProcessADT process);
void set_position(ProcessADT process, uint32_t position);
uint32_t get_position(ProcessADT process);
void free_process(ProcessADT process);
ProcessADT copy_process(ProcessADT process, Function function, char ** args);
//esta desps ponerla en algun otro lugar, no aca
void strcopy(char* destination, char* string){
    if(destination == 0){
        return;
    }
    int i;
    for(i  = 0; string[i] != '\0'; i++){
        destination[i] = string[i];
    }
    destination[i] = '\0';
}

//esta desps ponerla en algun otro lugar, no aca
int strlen(const char * s) {
    int i = 0;
    while(s[i] != '\0') {
        if(s[i] == '\t') {
            i+=4;
        }
        i++;
    }
    return i;
}


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
