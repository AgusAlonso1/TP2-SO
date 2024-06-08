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

#define READ 'r'
#define WRITE 'w'


typedef struct PipeMasterCDT* PipeMasterADT;
typedef struct PipeCDT* PipeADT;


void createPipeMaster();
PipeMasterADT getPipeMaster();
PipeADT createPipe(uint64_t id);
Node * getPipeNodeById(uint64_t id);
uint16_t pipeOpen(uint64_t id, char mode);
uint16_t pipeOpenAnonymous(uint64_t id, char mode, uint32_t pid);
uint16_t pipeClose(uint64_t id);
uint16_t pipeCloseAnonymous(uint64_t id, uint32_t pid);
uint16_t pipeWrite(uint64_t id, uint32_t pid, char* msg, int len);
uint16_t pipeRead(uint64_t id, uint32_t pid, char* buffer, int len);

#endif