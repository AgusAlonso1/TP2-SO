#ifndef PIPE_MASTER_H
#define PIPE_MASTER_H

#include "scheduler.h"
#include "definitions.h"
#include <stddef.h>

#define MAX_BUFFER_LEN 256

#define NOT_DEFINED (-1)
#define FOUND 1
#define NOT_FOUND 0
#define EOF (-1)



typedef struct PipeMasterCDT* PipeMasterADT;
typedef struct PipeCDT* PipeADT;


void createPipeMaster();
PipeMasterADT getPipeMaster();
PipeADT createPipe(uint64_t id);
Node * getPipeNodeById(uint64_t id);
int16_t pipeOpen(int id, char mode);
int16_t pipeOpenAnonymous(int id, char mode, uint32_t pid);
int16_t pipeClose(int id);
int16_t pipeCloseAnonymous(int id, uint32_t pid);
int16_t pipeWrite(uint64_t id, uint32_t pid, char* msg, int len);
int16_t pipeRead(uint64_t id, uint32_t pid, char* buffer, int len, uint32_t * readBytes);
int64_t getPipeId();

#endif