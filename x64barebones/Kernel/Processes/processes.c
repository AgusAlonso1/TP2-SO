#include <processes.h>
#include <stddef.h>
#include <scheduler.h>

extern void * _create_stack_frame(wrp wrapperFuntion, Function function, void * stackEnd, char ** args);

/* Comentarios process:
 *
 */


typedef struct ProcessCDT {
    uint32_t pid;
    uint32_t parentPid;
    uint32_t watingPid;
    char inmortal;
    char * name; //?uint32_t
    uint64_t priority; //?
    uint64_t state;
    void * stack;
    void * basePointer;
    char position;  //Background (1) or Foreground (0)
    uint64_t returnValue;  //no se si inicializarlo o dejarlo asi
    LinkedListADT deadChildren;
    //uint64_t fileDescriptors[3];
} ProcessCDT;

void wrapper(Function function, char **args);

ProcessADT createProcess(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, char inmortal, char position, Function function, char **args) {
    ProcessADT process = allocMemory(sizeof(ProcessCDT)); //funcion proxima a ser creada
    process->pid = pid;
    process->parentPid = parentPid;
    process->watingPid = 0;
    process->inmortal = inmortal;
    process->name =  allocMemory(sizeof(my_strlen(name))+1);
    my_strcopy(process->name, name);
    process->priority = priority;
    process->state = READY;
    process->position = position;   //foreground or background
    process->basePointer = allocMemory(STACK_SIZE);
    void* stackEnd = (void*) ((uint64_t)process->basePointer + STACK_SIZE);
    char** arguments = allocMemory(sizeof(args));
    argscopy(arguments, args);
    process->stack = _create_stack_frame(&wrapper, function, stackEnd, arguments);
    process->deadChildren = createLinkedList();
    //process->fileDescriptors[0] =
    return process;
}


void wrapper(Function function, char **args) {
    int len = stringArrayLen(args);
    int ret = function(len, args);     //todas las funciones reciben un argc y argv; el valor que retorna la funcion lo guardamos por si otro proceso lo necesita
    exitProcess(ret);
}


void setProcessState(ProcessADT process, uint64_t state) {
    if(process->state != state) {
        process->state = state;
    }
} 

uint64_t getProcessState(ProcessADT process){
    return process->state;
}

void setProcessParentPid(ProcessADT process, uint32_t parentPid) {
    process->parentPid = parentPid;
}

uint32_t getProcessParentPid(ProcessADT process){
    return process->parentPid;
}

void setProcessPid(ProcessADT process, uint32_t pid) {
    process->pid = pid;
}

uint32_t getProcessPid(ProcessADT process){
    return process->pid;
}

void setProcessPriority(ProcessADT process, uint32_t priority) {
    process->priority = priority;
}

uint32_t getProcessPriority(ProcessADT process){
    return process->priority;
}

void setProcessPosition(ProcessADT process, uint32_t position) {
    process->position = position;
}

uint32_t getProcessPosition(ProcessADT process){
    return process->position;
}

void freeProcess(ProcessADT process){
    freeMemory(process->name);
    freeMemory(process->basePointer);
    freeMemory(process->deadChildren);
    freeMemory(process);
}



ProcessCopy * copyProcess(ProcessCopy * processCopy , ProcessADT process){
    processCopy->name =  allocMemory(sizeof(my_strlen(process->name)+1));
    my_strcopy(processCopy->name, process->name);

    processCopy->pid = process->pid;
    processCopy->priority = process->priority;
    processCopy->basePointer = process->basePointer;
    processCopy->stack = process->stack;
    processCopy->state = process->state;
    processCopy->position = process->position;

    return processCopy;
}

void setProcessStack(ProcessADT process, void * stack) {
    process->stack = stack;
}

void * getProcessStack(ProcessADT process) {
    return process->stack;
}

void setProcessReturnValue(ProcessADT process, int returnValue) {
    process->returnValue = returnValue;
}

uint64_t getProcessReturnValue(ProcessADT process) {
    return process->returnValue;
}

LinkedListADT getProcessDeadChildList(ProcessADT process) {
    return process->deadChildren;
}

int getProcessMortality(ProcessADT process) {
    return process->inmortal;
}

uint32_t getProcessWatingPid(ProcessADT process) {
    return process->watingPid;
}

void setProcessWaitingPid(ProcessADT process, uint32_t childPid) {
    process->watingPid = childPid;
}

void argscopy(char** arguments, char** args){
    uint64_t  argc = my_atoi(args[0]); //supongo el primer argumetno es siempre argc
    uint32_t totalArgsDim = 0;
    //calculamos la cantidad de memoria que vamos a tener q allocar
    for(int i = 0; i < argc; i++){
        totalArgsDim += my_strlen(args[i]) + 1;
    }
    arguments = allocMemory(totalArgsDim + sizeof(char **) * (argc + 1));   //memoria para almacenar los strings y para almacenar los punteros a dichos strings
    char *pointer = (char *) arguments + (sizeof(char **) * (argc + 1));

    for(int i = 0; i < argc; i++){
        arguments[i] = pointer;
        memcpy(pointer, args[i], my_strlen(args[i]) + 1);
        pointer += my_strlen(args[i]) + 1;
    }
    arguments[argc] = NULL;
}




