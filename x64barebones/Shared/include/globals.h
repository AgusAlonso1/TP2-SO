#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdint.h>

typedef int (*Function)(int, char **);

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define ZOMBIE 3
#define BLOCK_UNBLOCK 4
#define UNBLOCK_BLOCK 5

#define CANT_FILE_DESCRIPTORS 3

#define DEV_NULL -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ_FD 0
#define WRITE_FD 1
#define ERROR_FD 2


#define READ_MODE 'r'
#define WRITE_MODE 'w'


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