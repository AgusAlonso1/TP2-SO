// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pipeMaster.h"

#define GET_INDEX(index) ((index) % MAX_BUFFER_LEN)

typedef struct PipeCDT {
    int id;
    char buffer[MAX_BUFFER_LEN];
    int writePid;
    int readPid;
    int64_t semWrite;
    int64_t semRead;
    uint64_t writePos;
    uint64_t readPos;
} PipeCDT;

typedef struct PipeMasterCDT{
    LinkedListADT pipes;
    int idCount;
    uint64_t pipesQty;
} PipeMasterCDT;

void createPipeMaster() {
    PipeMasterADT pMaster = (PipeMasterADT) PIPE_MASTER_ADDRESS;
    pMaster->pipes = createLinkedList();
    pMaster->idCount = CANT_FILE_DESCRIPTORS + 1;
    pMaster->pipesQty = 0;
}

int getPipeId(){
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
    while(i < MAX_BUFFER_LEN){
        pipe->buffer[i] = 0;
        i++;
    }
    pipe->writePid = NOT_DEFINED;
    pipe->readPid = NOT_DEFINED;
    pipe->writePos = 0;
    pipe->readPos = 0;
    int64_t semIdWrite = getPipeId();
    int64_t semIdRead = getPipeId();
    if(semOpen(MAX_BUFFER_LEN, semIdWrite) == -1 || semOpen(0, semIdRead) == -1){
        return NULL;
    }
    pipe->semWrite = semIdWrite;
    pipe->semRead = semIdRead;

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

int16_t pipeOpen(int id, char mode) {
    return pipeOpenAnonymous(id, mode, getCurrentPid());
}

int16_t pipeOpenAnonymous(int id, char mode, uint32_t pid) {
    PipeMasterADT pMaster = getPipeMaster();

    if(id < CANT_FILE_DESCRIPTORS + 1){
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

int16_t pipeClose(int id) {
    return pipeCloseAnonymous(id, getCurrentPid());
}

int16_t pipeCloseAnonymous(int id, uint32_t pid) {
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
        semClose(pipe->semWrite);
        semClose(pipe->semRead);
        removeNode(pMaster->pipes, pipeNode);
        freeMemory(pipe);
        pMaster->pipesQty--;
    } else {
        return ERROR;
    }

    return SUCCESS;
}

int16_t pipeWrite(uint64_t id, uint32_t pid, char* msg, int len) {
    Node * pipeNode = getPipeNodeById(id);
    PipeADT pipe;

    if(pipeNode == NULL || len == 0) {
        return ERROR;
    } else {
        pipe = (PipeADT) pipeNode->data;
    }

    if(pipe->writePid != pid) {
        return ERROR;
    }

    int i = 0;
    while(i < len  && pipe->buffer[GET_INDEX(pipe->writePos)] != EOF){
        semWait(pipe->semWrite);

       if(isProcessAlive(pipe->writePid) == ERROR){
           return ERROR;
       }

       pipe->buffer[GET_INDEX(pipe->writePos)] = msg[i];
       pipe->writePos++;
       i++;

        semPost(pipe->semRead);
    }

    return SUCCESS;
}


int16_t pipeRead(uint64_t id, uint32_t pid, char* buffer, int len, uint32_t * readBytes) {
    Node * pipeNode = getPipeNodeById(id);
    PipeADT pipe;

    if(pipeNode == NULL || len == 0) {
        return ERROR;
    } else {
        pipe = (PipeADT) pipeNode->data;
    }

    if(pipe->readPid != pid) {
        return ERROR;
    }

    int i = 0;
    while(i < len  && pipe->buffer[GET_INDEX(pipe->readPos - 1)] != EOF){
        semWait(pipe->semRead);

        if(isProcessAlive(pipe->readPid) == ERROR){
            return ERROR;
        }

        buffer[i] = pipe->buffer[GET_INDEX(pipe->readPos)];
        pipe->readPos++;
        i++;

        *readBytes = i;

        semPost(pipe->semWrite);
    }

    return SUCCESS;
}






