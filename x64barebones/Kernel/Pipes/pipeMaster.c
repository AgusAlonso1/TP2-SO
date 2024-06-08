#include "pipeMaster.h"

#define GET_INDEX(index) ((index) % MAX_BUFFER_LEN)

typedef struct PipeCDT {
    int id;
    char buffer[MAX_BUFFER_LEN];
    int writePid;
    int readPid;
//  sem_t semWrite;  //semaforo para escribir, se inicializa en MAX_BUFFER_LEN
//  sem_t semRead;  //semaforo para leer, se incializa en 0
    uint64_t writePos;
    uint64_t readPos;
} PipeCDT;

typedef struct PipeMasterCDT{
    LinkedListADT pipes;
    uint64_t idCount;
    uint64_t pipesQty;
} PipeMasterCDT;

void createPipeMaster() {
    PipeMasterADT pMaster = (PipeMasterADT) PIPE_MASTER_ADDRESS;
    pMaster->pipes = createLinkedList();
    pMaster->idCount = CANT_FILE_DESCRIPTORS + 1;
    pMaster->pipesQty = 0;
}

uint64_t getPipeId(){
    PipeMasterADT pMaster = getPipeMaster();
    return pMaster->idCount++;
}

PipeMasterADT getPipeMaster() {
    return (PipeMasterADT) PIPE_MASTER_ADDRESS;
}

PipeADT createPipe(uint64_t id) {
    PipeADT pipe = allocMemory(sizeof (PipeCDT));
    if(pipe == NULL){
        return NULL;
    }
    pipe->id = id;
    int i = 0;
    while(i < MAX_BUFFER_LEN){      //lo hice en un while porque por alguna razon no le gustaban los parentesis del for ???
        pipe->buffer[i] = 0;
        i++;
    }
    pipe->writePid = NOT_DEFINED;
    pipe->readPid = NOT_DEFINED;
    pipe->writePos = 0;
    pipe->readPos = 0;
//  sem_t semWrite = createSem(MAX_BUFFER_LEN, ...);  //crear el semaforo
//  sem_t semRead = createSem(0, ...);                //crear el semaforo
    return pipe;
}

Node * getPipeNodeById(uint64_t id) {
    PipeMasterADT pMaster = getPipeMaster();
    Node * currentNode = getFirst(pMaster->pipes);

    char found = NOT_FOUND;
    Node * pipeNode = NULL;
    while(currentNode != NULL && !found){
        PipeADT pipe = (PipeADT) currentNode->data;
        if(pipe->id == id){
            found = FOUND;
            pipeNode = currentNode;
        }
        currentNode = currentNode->next;
    }
    return pipeNode;
}

uint16_t pipeOpen(uint64_t id, char mode) {     //aca no hay pid; el pid es el del process que esta RUNNING
    return pipeOpenAnonymous(id, mode, getCurrentPid());
}

uint16_t pipeOpenAnonymous(int id, char mode, uint32_t pid) {  // si no esta creado lo creo y lo meto a la lista
    PipeMasterADT pMaster = getPipeMaster();

    if(id < CANT_FILE_DESCRIPTORS + 1){         //si es de las entradas estandar no hacemos nada
        return SUCCESS;
    }

    Node * pipeNode = getPipeNodeById(id);
    PipeADT pipe;

    if(pipeNode == NULL) {
        pipe = createPipe(id);
        if(pipe == NULL){
            return ERROR;
        }
        insert(pMaster->pipes, pipe);
        pMaster->pipesQty++;
    } else {
        pipe = (PipeADT) pipeNode->data;
    }

    if(mode == READ_MODE){
        if(pipe->readPid != NOT_DEFINED){
            return ERROR;
        }
        pipe->readPid = pid;
    } else if(mode == WRITE_MODE){
        if(pipe->writePid != NOT_DEFINED){
            return ERROR;
        }
        pipe->writePid = pid;
    }
    return SUCCESS;
}

uint16_t pipeClose(uint64_t id) {       //aca no hay pid; el pid es el del process que esta RUNNING
    return pipeCloseAnonymous(id, getCurrentPid());
}

uint16_t pipeCloseAnonymous(int id, uint32_t pid) {
    PipeMasterADT pMaster = getPipeMaster();
    Node * pipeNode = getPipeNodeById(id);
    PipeADT pipe;

    if(pipeNode == NULL) {
        return ERROR;
    } else {
        pipe = (PipeADT) pipeNode->data;
    }

    if(pipe->writePid == pid){
        char endOfFile[1] = {EOF};
        pipeWrite(id, pid, endOfFile, 1);
        pipe->writePid = NOT_DEFINED;
    } else if(pipe->readPid == pid){
        removeNode(pMaster->pipes, pipeNode);
        freeMemory(pipe);
        pMaster->pipesQty--;
    } else {
        return ERROR;
    }

    return SUCCESS;
}

uint16_t pipeWrite(uint64_t id, uint32_t pid, char* msg, int len) {
    Node * pipeNode = getPipeNodeById(id);
    PipeADT pipe;

    if(pipeNode == NULL || len == 0) {
        return ERROR;
    } else {
        pipe = (PipeADT) pipeNode->data;
    }

    if(pipe->writePid != pid) {     // el pid que quiere escribir en este pipe no tiene los permisos
        return ERROR;
    }

    int i = 0;
    while(i < len  && pipe->buffer[GET_INDEX(pipe->writePos)] != EOF){
       // semWait(pipe->semWrite);      //semaforo para ver si puedo escribir

       if(isProcessAlive(pipe->writePid) == -1){     //chequeo que si se quedo bloqueado no se haya muerto antes de escribir (funcion que agregue a scheduler)
           return ERROR;
       }

       pipe->buffer[GET_INDEX(pipe->writePos)] = msg[i];
       pipe->writePos++;
       i++;

       // semPost(pipe->semRead):       //semaforo para marcar que hay algo para leer
    }

    return SUCCESS;
}


uint16_t pipeRead(uint64_t id, uint32_t pid, char* buffer, int len, uint32_t * readBytes) {
    Node * pipeNode = getPipeNodeById(id);
    PipeADT pipe;

    if(pipeNode == NULL || len == 0) {
        return ERROR;
    } else {
        pipe = (PipeADT) pipeNode->data;
    }

    if(pipe->readPid != pid) {     // el pid que quiere leer en este pipe no tiene los permisos
        return ERROR;
    }

    int i = 0;
    while(i < len  && pipe->buffer[GET_INDEX(pipe->readPos)] != EOF){
        // semWait(pipe->semRead);      //semaforo para ver si puedo leer

        if(!isProcessAlive(pipe->readPid)){     //chequeo que si se quedo bloqueado no se haya muerto antes de escribir
            return ERROR;
        }

        buffer[i] = pipe->buffer[GET_INDEX(pipe->readPos)];
        pipe->readPos++;
        i++;

        if(buffer[i] == '\0'){
            *readBytes = 0;
        } else{
            *readBytes = i;
        }

        // semPost(pipe->semWrite):       //semaforo para marcar que hay lugar para escribir
    }

    return SUCCESS;
}






