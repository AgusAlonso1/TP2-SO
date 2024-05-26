#include <stddef.h>
#include <globals.h>


typedef struct ProcessCopyListCDT {
    uint64_t length;
    ProcessCopy * processCopyList;
} ProcessCopyListCDT;


void setProcessCopyListLength(ProcessCopyListADT processCopy, uint64_t length){
    processCopy->length = length;
}

void setProcessCopyList(ProcessCopyListADT processCopy, ProcessCopyADT processList){
    processCopy->processCopyList = processCopyList;
}

static int stringArrayLen(char **args) {
    int len = 0;
    while (args[len] != NULL) {
        len++;
    }
    return len;
}

static uint64_t my_atoi(char *s) {
    uint64_t acum = 0;
    uint64_t factor = 1;

    if(*s == '-') {
        factor = -1;
        s++;
    }

    while((*s >= '0')&&(*s <= '9')) {
        acum = acum * 10;
        acum = acum + (*s - 48);
        s++;
    }
    return (factor * acum);
}
//esta desps ponerla en algun otro lugar, no aca
static void strcopy(char* destination, char* string){
    if(destination == 0){
        return;
    }
    int i;
    for(i  = 0; string[i] != '\0'; i++){
        destination[i] = string[i];
    }
    destination[i] = '\0';
}

//esta desps ponerla en algun otro lugar, no aca
static int strlen(const char * s) {
    int i = 0;
    while(s[i] != '\0') {
        if(s[i] == '\t') {
            i+=4;
        }
        i++;
    }
    return i;
}

