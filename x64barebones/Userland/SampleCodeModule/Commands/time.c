// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>

void time(){
    uint8_t * currentTime;
    call_get_time(&currentTime);

    printf("%s\n", (char *) currentTime);
    return;
}



