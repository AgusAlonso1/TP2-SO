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
#define EOF (-1)

#define READ_MODE 'r'
#define WRITE_MODE 'w'

#define FOREGROUND 1
#define BACKGROUND 0

#define ERROR (-1)
#define SUCCESS 0

#define IDLE 0
#define SHELL 1

#define PRIORITY_LEVELS 5

enum levels {
    LEVEL0 = 0,
    LEVEL1 = 1,
    LEVEL2 = 2,
    LEVEL3 = 3,
    LEVEL4 = 4
};

typedef struct ProcessCopy {
    uint32_t pid;
    char * name;
    uint64_t priority;
    uint64_t state;
    void * stack;
    void * basePointer;
    char position;
} ProcessCopy;

typedef struct MemoryData {
    char * typeName;
    uint64_t memoryInUse;
    uint64_t memoryFree;
} MemoryData;


typedef struct ProcessCopyListCDT * ProcessCopyListADT;
typedef struct ProcessCopyList {
    uint64_t length;
    ProcessCopy * processCopyList;
} ProcessCopyList;

#endif