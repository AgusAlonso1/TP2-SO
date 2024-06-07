#include <commands.h>

char *states[] = {"Running", "Ready", "Blocked", "Zombie"};

int ps(int argc, char** argv) {
    ProcessCopyList * processCopyList = call_get_process_copy();

    if (processCopyList == NULL) {
        return -1;
    }



    for (int i = 0; i < processCopyList->length; i++) {
        ProcessCopy *processCopy = &processCopyList->processCopyList[i];
        printf(" NAME: %s PID: %d PRIORITY: %d STATE: %s ", processCopy->name, processCopy->pid, (int) processCopy->priority, states[processCopy->state]);
        printf("STACK: %p BASE: %p POSITION: %s \n", processCopy->stack, processCopy->basePointer, processCopy->position == 1 ? "foreground" : "background");
        call_free(processCopyList->processCopyList[i].name);
    }
    call_free(processCopyList->processCopyList);
    call_free(processCopyList);

    /*
    printf("name \t id \t priority \t state \t stack \t base \t foreground \n");


    for (uint64_t i = 0; i < length; i++) {
        ProcessCopy * processCopy = getProcessIndex(processCopyList, i);
        printf("%s \t %d \t %d \t %s \t %p \t %p \t %s \n", processCopy->name, processCopy->pid, (int) processCopy->priority, states[processCopy->state], processCopy->stack, processCopy->basePointer, processCopy->position == 1 ? "fg" : "bg");
    }
     */
    return 0;
}
