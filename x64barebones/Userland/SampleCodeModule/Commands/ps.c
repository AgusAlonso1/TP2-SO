#include <commands.h>

char *states[] = {"Running", "Ready", "Blocked", "Zombie"};

int ps(int argc, char** argv) {
    ProcessCopyList * processCopyList = call_get_process_copy();

    if (processCopyList == NULL) {
        return -1;
    }


    for (int i = 0; i < processCopyList->length; i++) {
        ProcessCopy *processCopy = &processCopyList->processCopyList[i];
        printf(" Name: %s PID: %d Priority: %d State: %s ", processCopyList->processCopyList[i].name, processCopy->pid, (int) processCopy->priority, states[processCopy->state]);
        printf("Stack Pointer: %p Base Pointer: %p Position: %s \n", processCopy->stack, processCopy->basePointer, processCopy->position == 1 ? "Foreground" : "Background");
    }

    call_free_process_copy(processCopyList);

    return 0;
}
