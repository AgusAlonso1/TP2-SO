#include <commands.h>

int minfo(int argc, char** argv) {
    MemoryData * memoryData = call_get_mem_info();
    printf("Memory type: %s --- Free bytes: %d --- Allocated bytes: %d\n", memoryData->typeName, memoryData->memoryFree, memoryData->memoryInUse);
    return 0;
}