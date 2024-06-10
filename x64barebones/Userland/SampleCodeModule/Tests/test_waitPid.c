#include <stdint.h>
#include "test_util.h"
#include <libc.h>
#include <syscallFunctions.h>

int childProcess(int argc, char *argv[]) {
    printf("Child process starting\n");
    call_sleep_seconds(2); // Sleep for 2 seconds to simulate some work
    printf("Child process finished\n");
    return 0;
}

void test_waitProcessPid() {
    printf("Parent process starting\n");
    int fileDescriptors[CANT_FILE_DESCRIPTORS] = {STDIN, STDOUT, STDERR};
    uint32_t test_pid = call_get_pid();
    uint32_t childPid = call_create_process_foreground("childProcess", &childProcess, NULL, test_pid, fileDescriptors);
    if (childPid < 0) {
        printf("Failed to create child process\n");
        return;
    }

    printf("Waiting for child process to finish\n");
    call_waitpid(childPid);
    printf("Child process has finished\n");
}