#include <commands.h>
static int loop2(int argc, char** argv);

int loop(int argc, char** argv) {
    if(argc < 1){
        return -1;
    }
    int ms = atoi(argv[0]);
    uint32_t pid = call_get_pid();
    while(1){
        printf("Hola soy el process: %d\n", pid);
        uint32_t childPid = call_create_process("childPid", 0, (Function) loop2, argv, pid);
        call_sleep(ms);
    }
}

static int loop2(int argc, char** argv){
    while(1){
        printf("Hola soy un hijo\n");

        call_sleep(10000);
    }
}
