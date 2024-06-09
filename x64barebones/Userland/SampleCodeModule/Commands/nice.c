#include <commands.h>

int nice(int argc, char** argv) {
    if(argc < 2){
        return -1;
    }
    int pid = atoi(argv[0]);
    int priority = atoi(argv[1]);

    uint64_t success = call_set_priority(pid, priority);
    if(success != 0){
        printf("The process %d has been changed of priority successfully \n", pid);
    } else {
        printf("The process %d could not change its priority \n", pid);
    }

    return 0;
}
