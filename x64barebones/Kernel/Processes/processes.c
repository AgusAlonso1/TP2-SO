#include <processes.h>
#include <stddef.h>
#include <scheduler.h>
#include <pipeMaster.h>

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
    int returnValue;  //no se si inicializarlo o dejarlo asi
    char ** arguments;
    int fileDescriptors[CANT_FILE_DESCRIPTORS];
} ProcessCDT;


ProcessADT createProcess(uint32_t parentPid, uint32_t pid, char * name, uint64_t priority, char immortal, char position, Function function, char **args, const int fileDescriptors[CANT_FILE_DESCRIPTORS]) {
    ProcessADT process = allocMemory(sizeof(ProcessCDT));
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
    process->arguments = NULL;
    argscopy(&process->arguments, args);
    process->stack = _create_stack_frame(&wrapper, function, stackEnd, (void *) process->arguments);
    process->fileDescriptors[READ_FD] = fileDescriptors[READ_FD];
    process->fileDescriptors[WRITE_FD] = fileDescriptors[WRITE_FD];
    process->fileDescriptors[ERROR_FD] = fileDescriptors[ERROR_FD];

    pipeOpenAnonymous(process->fileDescriptors[READ_FD], READ_MODE, pid);
    pipeOpenAnonymous(process->fileDescriptors[WRITE_FD], WRITE_MODE, pid);
    pipeOpenAnonymous(process->fileDescriptors[ERROR_FD], WRITE_MODE, pid);

    return process;
}


void wrapper(Function function, char **args) {
    int len = stringArrayLen(args);
    int ret = function(len, args);
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

uint32_t getProcessParentPid(ProcessADT process){
    return process->parentPid;
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

uint32_t getProcessPosition(ProcessADT process){
    return process->position;
}

int freeProcess(ProcessADT process){
    if (process == NULL) {
        return -1;
    }
    if (process->arguments != NULL) {
        for (int i = 0; process->arguments[i] != NULL; i++) {
            freeMemory(process->arguments[i]);
        }
        freeMemory(process->arguments);
    }
    freeMemory(process->name);
    freeMemory(process->basePointer);
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

int getProcessReturnValue(ProcessADT process) {
    return process->returnValue;
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

int getProcessReadFileDescriptor(ProcessADT process){
    return process->fileDescriptors[READ_FD];
}

int getProcessWriteFileDescriptor(ProcessADT process){
    return process->fileDescriptors[WRITE_FD];
}

int getProcessErrorFileDescriptor(ProcessADT process){
    return process->fileDescriptors[ERROR_FD];
}


void argscopy(char ***arguments, char **args) {
    if (args == NULL) {
        *arguments = NULL;
        return;
    }

    uint64_t argc = stringArrayLen(args);

    *arguments = allocMemory(sizeof(char *) * (argc + 1));
    if (*arguments == NULL) {
        return;
    }

    for (int i = 0; i < argc; i++) {
        (*arguments)[i] = allocMemory(sizeof(char) * (my_strlen(args[i]) + 1));
        if ((*arguments)[i] == NULL) {
            for (uint64_t j = 0; j < i; j++) {
                freeMemory((*arguments)[j]);
            }
            freeMemory(*arguments);
            *arguments = NULL;
            return;
        }
        my_strcopy((*arguments)[i], args[i]);
    }
    (*arguments)[argc] = NULL;
}




