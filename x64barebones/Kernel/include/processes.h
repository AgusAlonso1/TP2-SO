#ifndef PROCESS_H
#define PROCESS_H 

#include <stdint.h>
#include <base_memory_manager.h>

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define FOREGROUND 0
#define BACKGROUND 1

typedef struct processCDT * processADT;

processADT create_process();
uint64_t set_state(uint64_t state);
uint64_t get_state();
uint64_t kill_process();


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
