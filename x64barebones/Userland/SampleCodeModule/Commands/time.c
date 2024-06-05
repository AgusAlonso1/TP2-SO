#include <commands.h>

void time(){
    uint8_t * currentTime;
    call_get_time(&currentTime);

    printf("%s\n", (char *) currentTime);
    return;
}



