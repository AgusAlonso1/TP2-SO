#include <processes.h>
#include <stddef.h>

extern void * _create_stack_frame(wrp wrapperFuntion, Function function, void * stackEnd, char ** args);

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

void wrapper(Function function, char **args);

static int stringArrayLen(char **args) {
    int len = 0;
    while (args[len] != NULL) {
        len++;
    }
    return len;
}

static uint64_t my_atoi(char *s) {
    uint64_t acum = 0;
    uint64_t factor = 1;
    
    if(*s == '-') {
        factor = -1;
        s++;
    }
    
    while((*s >= '0')&&(*s <= '9')) {
      acum = acum * 10;
      acum = acum + (*s - 48);
      s++;
    }
    return (factor * acum);
}
//esta desps ponerla en algun otro lugar, no aca
static void strcopy(char* destination, char* string){
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
static int strlen(const char * s) {
    int i = 0;
    while(s[i] != '\0') {
        if(s[i] == '\t') {
            i+=4;
        }
        i++;
    }
    return i;
}

//Hay q terminar de implementarlo!!!
// static void exitProcess(ProcessADT process){
//     setProcessState(process, EXITED);
//     freeProcess(process);
// }

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
    char** arguments = allocMemory(sizeof(args));
    argscopy(arguments, args);
    process->stack = _create_stack_frame(&wrapper, function, stackEnd, arguments);

    //process->fileDescriptors[0] =
    return process;
}


void wrapper(Function function, char **args) {
    int len = stringArrayLen(args);
    int ret = function(len, args);     //todas las funciones reciben un argc y argv; el valor que retorna la funcion lo guardamos por si otro proceso lo necesita
    //exitProcess(ret);      //todo: hay q implementar; seria un pongo el proceso en modo zombie (se supone que se sale el current)
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
    freeMemory(process);
}



//a chequear si funciona el tema de stack y basepointer
ProcessADT copyProcess(ProcessADT process, Function function, char ** args){
    ProcessADT new_process = createProcess(process->pid, process->parentPid, process->name, process->priority, process->state, process->position, function, args);
    new_process->stack = process->stack;
    new_process->basePointer = process->basePointer;
    return new_process;
}

void setProcessStack(ProcessADT process, void * stack) {
    process->stack = stack;
}

void * getProcessStack(ProcessADT process) {
    return process->stack;
}


void argscopy(char** arguments, char** args){
    uint64_t  argc = my_atoi(args[0]); //supongo el primer argumetno es siempre argc
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




