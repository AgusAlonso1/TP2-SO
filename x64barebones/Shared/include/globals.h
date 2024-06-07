#ifndef GLOBALS_H
#define GLOBALS_H
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

typedef struct ProcessCopyList {
    uint64_t length;
    ProcessCopy * processCopyList;
} ProcessCopyList;


#endif