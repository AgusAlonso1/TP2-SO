#include <commands.h>

void ps(int argc, char** argv) {
    ProcessCopyListADT processCopyList = call_get_process_copy();
    uint64_t size = getProcessCopyListLength(processCopyList);

    printf("name \t id \t priority \t state \t stack \t base \t foreground \n");

    if (list == NULL || list->processCopyList == NULL) {
        return;
    }

    for (uint64_t i = 0; i < list->length; i++) {
        ProcessCopy * processCopy = list->processCopyList[i];
        printf("%s \t %d \t %d \t %s \t %p \t %p \t %s \n", processCopy->name, processCopy->pid, (int) processCopy->priority, states[processCopy->state], processCopy->stack, processCopy->basePointer, processCopy->position == 1 ? "fg" : "bg");
    }

}
