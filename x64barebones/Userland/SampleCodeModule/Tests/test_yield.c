#include <stdint.h>
#include "test_util.h"
#include <libc.h>
#include <syscallFunctions.h>


// Función de prueba para el proceso A
void processA(int argc, char *argv[]) {
    for (int i = 0; i < 10; i++) {
        printf("Process A: %d\n", i);
        call_yield(); // Llama a yield para ceder la CPU
        printf("Process A: %d\n end", i);
    }
}

// Función de prueba para el proceso B
void processB(int argc, char *argv[]) {
    for (int i = 0; i < 10; i++) {
        printf("Process B: %d\n", i);
        call_yield(); // Llama a yield para ceder la CPU
        printf("Process B: %d\n end", i);
    }
}

// Función principal de prueba
void test_yield() {
    uint32_t test_pid = call_get_pid();
    uint64_t pids[2];
    int fileDescriptors[CANT_FILE_DESCRIPTORS] = {STDIN, STDOUT, STDERR};
    // Crear los procesos de prueba
    pids[0] = call_create_process_foreground("processA", &processA, NULL, test_pid, fileDescriptors);
    pids[1] = call_create_process_foreground("processB", &processB, NULL, test_pid, fileDescriptors);

    // Esperar a que los procesos terminen
    call_waitpid(pids[0]);
    call_waitpid(pids[1]);

    printf("Test completed.\n");
}