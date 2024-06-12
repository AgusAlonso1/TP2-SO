// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <base_memory_manager.h>

char * current_address = (char *) BASE_ADDRESS;

void * my_malloc() {
    if (current_address + CHUNCK_SIZE < (char *) MAX_ADRESS) {
        char * to_return = current_address;
        current_address += CHUNCK_SIZE;
        return to_return;
    } else {
        return NULL;
    }
}

