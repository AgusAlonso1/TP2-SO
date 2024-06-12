// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>


int kill(int argc, char** argv) {
    if(argc < 1){
        return -1;
    }
    int pid = atoi(argv[0]);

    uint64_t success = call_kill_process(pid);
    if(success != -1 ){
        printf("The process %d has been killed successfully \n", pid);
    } else{
        fprintf(STDERR,"The process %d could not be killed \n", pid);
    }

    return 0;
}
