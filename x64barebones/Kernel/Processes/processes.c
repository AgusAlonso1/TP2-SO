#include <processes.h>

/* Comentarios process:
 * Revisar el tema de copy process
 * Cambie todos los nombres de las funciones para que queden camel case, porque en algunos archivos estaba snake y en otros camel
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

ProcessADT createProcess(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, uint64_t state, char position, Function function, char **args) {
    ProcessADT process = allocMemory(sizeof(ProcessCDT)); //funcion proxima a ser creada
    process->pid = pid;
    process->parentPid = parentPid;
    process->name =  allocMemory(sizeof(strlen(name)));
    strcopy(process->name, name);
    process->priority = priority;
    process->state = READY;
    process->position = position;   //foreground or background
    process->basePointer = allocMemory(STACK_SIZE);
    void* stackEnd = (void*) ((uint64_t)process->basePointer + STACK_SIZE);
    char** arguments;
    argscopy(arguments, args);
    process->stack = _create_stack_frame(&wrapper, function, stackEnd, args); //todo: en realidad args deberia de ser una copia local

    //process->fileDescriptors[0] =
    return process;
}

void wrapper(Function function, char **args) {
    int len = stringArrayLen(args);
    int ret = function(len, args);     //todas las funciones reciben un argc y argv; el valor que retorna la funcion lo guardamos por si otro proceso lo necesita
    exitProcess(ret);      //todo: hay q implementar; seria un pongo el proceso en modo zombie (se supone que se sale el current)
}


void setState(ProcessADT process, uint64_t state) {
    if(process->state != state) {
        process->state = state;
    }
} 

uint64_t getState(ProcessADT process){
    return process->state;
}

void setParentPid(ProcessADT process, uint32_t parentPid) {
    process->parentPid = parentPid;
}

uint32_t getParentPid(ProcessADT process){
    return process->parentPid;
}

void setProcessPriority(ProcessADT process, uint32_t priority) {
    process->priority = priority;
}

uint32_t getPriority(ProcessADT process){
    return process->priority;
}

void setPosition(ProcessADT process, uint32_t position) {
    process->position = position;
}

uint32_t getPosition(ProcessADT process){
    return process->position;
}

void freeProcess(ProcessADT process){
    freeMemory(process->name);
    freeMemory(process->basePointer);
    freeMemory(process);
}



//a chequear si funciona el tema de stack y basepointer
ProcessADT copyProcess(ProcessADT process, Function function, char ** args){
    ProcessADT new_process = createProcess(process->pid, process->parentPid, process->name, process->priority, process->state, process->position, function, args);
    new_process->stack = process->stack;
    new_process->basePointer = process->basePointer;
    return new_process;
}

void setStack(ProcessADT process, void * stack) {
    process->stack = stack;
}

void * getStack(ProcessADT process) {
    return process->stack;
}

void argscopy(char** arguments, char** args){
    uint32_t argc = args[0]; //supongo el primer argumetno es siempre argc
    uint32_t totalArgsDim = 0;
    //calculamos la cantidad de memoria que vamos a tener q allocar
    for(int i = 0; i < argc; i++){
        totalArgsDim += strlen(args[i]) + 1;
    }
    arguments = allocMemory(totalArgsDim + sizeof(char **) * (argc + 1));   //memoria para almacenar los strings y para almacenar los punteros a dichos strings
    char *pointer = (char *) arguments + (sizeof(char **) * (argc + 1));

    for(int i = 0; i < argc; i++){
        arguments[i] = pointer;
        memcpy(pointer, args[i], strlen(args[i]) + 1);
        pointer += strlen(args[i]) + 1;
    }
    arguments[argc] = NULL;
}




