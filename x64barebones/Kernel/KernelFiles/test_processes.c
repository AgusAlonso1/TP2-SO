#include <stdio.h>
#include "syscall.h"
#include <processes.h>
#include <test_util.h>
#include <cursor.h>
#include <videoDriver.h>
#include <timer.h>
#include <memoryManager.h>

enum State { RUNNING1,
             BLOCKED1,
             KILLED1 };

typedef struct P_rq {
  ProcessADT process;
  int32_t pid;
  enum State state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  MemoryManagerADT memoryManager = getMemoryManager();

  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};\
  uint32_t hola = 40;
  initializeCursor(20,20, 2);

  if (argc != 1)
    return -1;

  if ((max_processes = satoi(argv[0])) <= 0)
    return -1;

  p_rq p_rqs[max_processes];

  while (1) {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      p_rqs[rq].process = createProcess(0, rq, "endless_loop", 3, 0, 0, &endless_loop, argvAux);
      p_rqs[rq].pid = getProcessPid(p_rqs[rq].process);
      int p = p_rqs[rq].pid; 
      if (p_rqs[rq].pid == -1) {
        drawStringOnCursor((uint8_t *)"test_processes: ERROR creating process\n", &hola);
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
        ProcessADT tomi = p_rqs[rq].process;
        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING1 || p_rqs[rq].state == BLOCKED1) {
              
              if (freeProcess(p_rqs[rq].process) == -1) {
                ("test_processes: ERROR killing process\n");
                return -1;
              }
              p_rqs[rq].state = KILLED1;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING1) {
              if (setProcessState(p_rqs[rq].process, BLOCKED) == -1) {
                drawStringOnCursor((uint8_t *) "test_processes: ERROR blocking process\n", &hola);
                return -1;
              }
              p_rqs[rq].state = BLOCKED1;
              ProcessADT tomi = p_rqs[rq].process;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED1 && GetUniform(100) % 2) {
          if (setProcessState(p_rqs[rq].process, RUNNING) == -1) {
            drawStringOnCursor((uint8_t *)"test_processes: ERROR unblocking process\n", &hola);
            return -1;
          }
          p_rqs[rq].state = RUNNING1;
          ProcessADT tomi = p_rqs[rq].process;
        }
    }
    break;
  }
    drawStringOnCursor((uint8_t *)"success\n", &hola);
    sleep(3000);
}
