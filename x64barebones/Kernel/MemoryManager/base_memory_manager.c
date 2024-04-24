#include <base_memory_manager.h>

char * current_address = BASE_ADDRESS;

void * my_malloc() {
    if (current_address + CHUNCK_SIZE < MAX_ADRESS) {
        char * to_return = current_address;
        current_address += CHUNCK_SIZE;
        return to_return;
    } else {
        return NULL;
    }
}

