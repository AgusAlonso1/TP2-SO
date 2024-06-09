#include <stdint.h>
#include <stdio.h>
#include "syscallFunctions.h"
#include "test_util.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 20000000     // TODO: Change this value to make the wait long enough to see these processes being run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {0};
    uint64_t i;

    for (i = 0; i < TOTAL_PROCESSES; i++){
        int fd[CANT_FILE_DESCRIPTORS] = {DEV_NULL, STDOUT, STDERR};
        pids[i] = call_create_process_background("endless_loop_print", (Function) endless_loop_print, argv, call_get_pid(), fd);
    }


    bussy_wait(WAIT);
    printf("\nCHANGING PRIORITIES...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        call_set_priority(pids[i], prio[i]);

    bussy_wait(WAIT);
    printf("\nBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        call_block(pids[i]);

    printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        call_set_priority(pids[i], MEDIUM);

    printf("UNBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        call_block(pids[i]);

    bussy_wait(WAIT);
    printf("\nKILLING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        call_kill_process(pids[i]);
}