#include <commands.h>


int kill(int argc, char** argv) {
    if(argc < 1){
        return -1;
    }
    int pid = atoi(argv[0]);

    uint64_t success = call_kill_process(pid);
    if(success != 0 ){
        printf("The process %d has been killed successfully \n", pid);
    } else{
        printf("The process %d could not be killed \n", pid);
    }

    return 0;
}
