#ifndef _GLOBALS_H
#define _GLOBALS_H
#include <stdint.h>

typedef int (*Function)(int, char **);

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define ZOMBIE 3

typedef struct ProcessCopy {
    uint32_t pid;
    char * name;
    uint64_t priority;
    uint64_t state;
    void * stack;
    void * basePointer;
    char position;
} ProcessCopy;

typedef struct ProcessCopyListCDT* ProcessCopyListADT;

void setProcessCopyListLength(ProcessCopyListADT processCopy, uint64_t length);
void setProcessCopyList(ProcessCopyListADT processCopy, ProcessCopyADT processLis);
static int stringArrayLen(char **args);
static uint64_t my_atoi(char *s);
static void strcopy(char* destination, char* string);


#endif