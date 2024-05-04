#include <processes.h>

typedef struct processCDT{
    uint32_t pid;
    uint32_t parentPid;
    char * name; //?uint32_t
    uint64_t priority; //?
    uint64_t state;
    uint64_t * stack;
    uint64_t * basePointer;
    char position;  //Background or Foreground
} processCDT;

