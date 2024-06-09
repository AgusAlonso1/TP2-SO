#include <stdio.h>
#include <syscallFunctions.h>
#include "test_util.h"

enum State { RUNNING1,
    BLOCKED1,
    KILLED1 };

typedef struct P_rq {
    int32_t pid;
    enum State state;
} p_rq;

int test_processes(int argc, char *argv[]) {
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {0};

    if (argc != 1)
        return -1;

    if ((max_processes = satoi(argv[0])) <= 0)
        return -1;

    p_rq p_rqs[max_processes];

    while (1) {

        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++) {
            int fd[CANT_FILE_DESCRIPTORS] = {DEV_NULL, STDOUT, STDERR};
            p_rqs[rq].pid = call_create_process_background("endless_loop", (Function) endless_loop, argvAux, call_get_pid(), fd);

            if (p_rqs[rq].pid == -1) {
                printf("test_processes: ERROR creating process\n");
                return -1;
            } else {
                p_rqs[rq].state = RUNNING1;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {

            for (rq = 0; rq < max_processes; rq++) {
                action = GetUniform(100) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == RUNNING1 || p_rqs[rq].state == BLOCKED1) {
                            if (call_kill_process(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR killing process\n");
                                return -1;
                            }
                            p_rqs[rq].state = KILLED1;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING1) {
                            if (call_block(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR blocking process\n");
                                return -1;
                            }
                            p_rqs[rq].state = BLOCKED1;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED1 && GetUniform(100) % 2) {
                    if (call_block(p_rqs[rq].pid) == -1) {
                        printf("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING1;
                }
        }
    }
}