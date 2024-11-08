// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>

int block(int argc, char** argv) {
    if(argc < 1){
        return -1;
    }
    int pid = atoi(argv[0]);


    uint64_t success = call_block(pid);
    if(success != -1){
        if(success == UNBLOCK_BLOCK){
            printf("The process %d has been blocked successfully \n", pid);
        }else {
            printf("The process %d has been unblocked successfully \n", pid);
        }
    } else {
        fprintf(STDERR,"The process %d could not be blocked/unblocked \n", pid);
    }

    return 0;
}
