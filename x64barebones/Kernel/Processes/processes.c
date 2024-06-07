#include <processes.h>
#include <stddef.h>
#include <scheduler.h>

/* Comentarios process:
 *
 */


typedef struct ProcessCDT {
    uint32_t pid;
    uint32_t parentPid;
    uint32_t waitingPid;
    char immortal;
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


ProcessADT createProcess(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, char immortal, char position, Function function, char **args) {
    ProcessADT process = allocMemory(sizeof(ProcessCDT)); //funcion proxima a ser creada
    process->pid = pid;
    process->parentPid = parentPid;
    process->waitingPid = 0;
    process->immortal = immortal;
    process->name =  allocMemory(my_strlen(name)+1);
    my_strcopy(process->name, name);
    process->priority = priority;
    process->state = READY;
    process->position = position;   //foreground or background
    process->basePointer = allocMemory(STACK_SIZE);
    void* stackEnd = (void*) ((uint64_t)process->basePointer + STACK_SIZE);
    char** arguments = NULL;
    argscopy(arguments, args);
    process->stack = _create_stack_frame(&wrapper, function, stackEnd, (void *) arguments);
    process->deadChildren = createLinkedList();
    //process->fileDescriptors[0] =
    return process;
}


void wrapper(Function function, char **args) {
    int len = stringArrayLen(args);
    int ret = function(len, args);     //todas las funciones reciben un argc y argv; el valor que retorna la funcion lo guardamos por si otro proceso lo necesita
    exitProcess(ret);
}


int setProcessState(ProcessADT process, uint64_t state) {
    if (process == NULL) {
        return -1;
    }
    if(process->state != state) {
        process->state = state;
    }
    return 0;
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

int freeProcess(ProcessADT process){
    if (process == NULL) {
        return -1;
    }
    freeMemory(process->name);
    freeMemory(process->basePointer);
    freeMemory(process->deadChildren);
    freeMemory(process);
    return 0;
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

char getProcessMortality(ProcessADT process) {
    return process->immortal;
}

uint32_t getProcessWaitingPid(ProcessADT process) {
    return process->waitingPid;
}

void setProcessWaitingPid(ProcessADT process, uint32_t childPid) {
    process->waitingPid = childPid;
}

void argscopy(char** arguments, char** args){
    uint64_t  argc = stringArrayLen(args);

    arguments = allocMemory(sizeof(char *) * (argc + 1));

    for(int i = 0; i < argc; i++){
        char * newArg = allocMemory(sizeof(char) * (my_strlen(args[i]) + 1));
        my_strcopy(newArg, args[i]);
        arguments[i] = newArg;
    }
    arguments[argc] = NULL;
}




