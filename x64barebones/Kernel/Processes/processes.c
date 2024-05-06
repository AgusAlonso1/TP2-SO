#include <processes.h>

/* Comentarios process:
 * Revisar el tema de copy process
 * La idea de la wrapper es como lo que dijo agodio en donde cada vez que termina de ejecutarse el proceso, manejamos
el valor de retorno y que se quede muerto
  * Como habias dicho tomi, me parece que lo de igualar char* y eso no va; va a ver que hacer copias de los valores que pasan por parametros (punteros)
porq ahi vi que dijo agodio que esos punteros pueden ser modificados en otro lado y romper las cosas en kernel
 * En assembler cree la funcion _create_stack_frame, que hace basicamente lo que dijo agodio en la clase (pushea las cosas y arma el stack)
 * Tambien defini un tipo de dato Function que seria un puntero a funcion, donde las funciones tienen (int argc, char **args). La idea desps
seria ponerla en algun .h que tambn se comparta con userland, asi podemos definirlas ahi y pasarlas por las syscalls directamente
 * Despues intente definir el tamaño del stack para conseguir el stackEnd, no se si el numero esta bien (intente hacer un registros*tmañoregistros)
 */


typedef struct ProcessCDT {
    uint32_t pid;
    uint32_t parentPid;
    char * name; //?uint32_t
    uint64_t priority; //?
    uint64_t state;
    void * stack;
    void * basePointer;
    char position;  //Background or Foreground
    //uint64_t fileDescriptors[3];
} ProcessCDT;

ProcessADT create_process(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, uint64_t state, char position, Function function, char **args) {
    ProcessADT process = allocMemory(sizeof(ProcessCDT)); //funcion proxima a ser creada
    process->pid = pid;
    process->parentPid = parentPid;
    process->name = name;   //todo: en realidad deberia de ser una copia local
    process->priority = priority;
    process->state = state;
    process->position = position;   //foreground or background
    process->basePointer = allocMemory(STACK_SIZE);
    void* stackEnd = (void*) ((uint64_t)process->stackBase + STACK_SIZE);
    process->stack = _create_stack_frame(&wrapper, function, stackEnd, args); //todo: en realidad args deberia de ser una copia local

    //process->fileDescriptors[0] =
    return process;
}

void wrapper(Function function, char **args) {
    int len = stringArrayLen(args);
    int ret = function(len, args);     //todas las funciones reciben un argc y argv; el valor que retorna la funcion lo guardamos por si otro proceso lo necesita
    exitProcess(ret);      //todo: hay q implementar; seria un pongo el proceso en modo zombie (se supone que se sale el current)
}


void set_state(ProcessADT process, uint64_t state) {
    if(process->state != state) {
        process->state = state;
    }
} 

uint64_t get_state(ProcessADT process){
    return process->state;
}

void set_parentPid(ProcessADT process, uint32_t parentPid) {
    process->parentPid = parentPid;
}

uint32_t get_parentPid(ProcessADT process){
    return process->parentPid;
}

void set_priority(ProcessADT process, uint32_t priority) {
    process->priority = priority;
}

uint32_t get_priority(ProcessADT process){
    return process->priority;
}

void set_position(ProcessADT process, uint32_t position) {
    process->position = position;
}

uint32_t get_position(ProcessADT process){
    return process->position;
}

void free_process(ProcessADT process){
    freeMemory(process->stack);
    freeMemory(process->basePointer);
    freeMemory(process);
}

//a chequear si funciona el tema de stack y basepointer
ProcessADT copy_process(ProcessADT process){
    processADT new_process = create_process(process->pid, process->parentPid, process->name, process->priority, process->state, process->position);
    new_process->stack = process->stack;
    new_process->basePointer = process->basePointer;
    return new_process;
}




