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

typedef struct ProcessCopyListCDT * ProcessCopyListADT;

void setProcessCopyListLength(ProcessCopyListADT processCopy, uint64_t length);
void setProcessCopyList(ProcessCopyListADT processCopy, ProcessCopy * processLis);

int my_strlen(const char * s);
void my_strcopy(char * destination, char* string);
uint64_t my_atoi(char *s);

int stringArrayLen(char **args);

// Seria mas prolijo retornar diractamente el puntero, pero tengo que incluir mas cosas asi que mientras solo retorno el tamano
uint32_t getProcessCopyListSize();

#endif