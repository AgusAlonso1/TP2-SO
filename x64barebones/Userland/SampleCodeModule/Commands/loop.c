// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>

int loop(int argc, char** argv) {
    if(argc < 1){
        return -1;
    }
    int seconds = atoi(argv[0]);
    uint32_t pid = call_get_pid();
    while(1){
        printf("Hola soy el process: %d\n", pid);
        call_sleep_seconds(seconds);
    }
    return 0;
}
