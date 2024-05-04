#include <processes.h>

typedef struct processCDT {
    uint32_t pid;
    uint32_t parentPid;
    char * name; //?uint32_t
    uint64_t priority; //?
    uint64_t state;
    uint64_t * stack;
    uint64_t * basePointer;
    char position;  //Background or Foreground
    //uint64_t fileDescriptors[3];
} processCDT;

processADT create_process(uint32_t pid, uint32_t parentPid, char * name, uint64_t priority, uint64_t state, char position) {
    processADT process = my_malloc();
    process->pid = pid;
    process->parentPid = parentPid;
    process->name = name;
    process->priority = priority;
    process->state = state;
    process->position = position;
    //process->fileDescriptors[0] = 
    //process->basePointer = 
    //process->stack = 

    return process;
}

uint64_t set_state(processADT process, uint64_t state) {
    if(process->state != state) {
        process->state = state;
    }
} 

uint64_t get_state(processADT process){
    return process->state;
}


