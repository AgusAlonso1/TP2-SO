#include <commands.h>

int block(int argc, char** argv) {
    if(argc < 1){
        return -1;
    }
    int pid = atoi(argv[0]);


    uint64_t success = call_block(pid);
    if(success != 0){
        if(success == UNBLOCK_BLOCK){
            printf("The process %d has been blocked successfully \n", pid);
        }else {
            printf("The process %d has been unblocked successfully \n", pid);
        }
    } else {
        printf("The process %d could not be blocked/unblocked \n", pid);
    }

    return 0;
}
