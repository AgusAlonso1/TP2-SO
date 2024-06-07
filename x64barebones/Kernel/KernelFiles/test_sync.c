#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include <cursor.h>
#include <videoDriver.h>
#include <test_util.h>
#include <processes.h>
#include <scheduler.h>
#include <semaphores.h>
#include <timer.h>


#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

int my_process_inc(int argc, char *argv[]) {
  uint64_t n = 10;
  int8_t inc = -1;
  int8_t use_sem = 1;
  uint64_t semId;
  uint32_t hola;

  if (argc != 3)
    return -1;

  // if ((n = satoi(argv[0])) <= 0)
  //   return -1;
  // if ((inc = satoi(argv[1])) == 0)
  //   return -1;
  // if ((use_sem = satoi(argv[2])) < 0)
  //   return -1;

  if (use_sem)
    if ((semId = semOpen(1)) < 0) {
      drawStringOnCursor((uint8_t *)"test_sync: ERROR opening semaphore\n", &hola);
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      semWait(semId);
    slowInc(&global, inc);
    if (use_sem)
      semPost(semId);
  }

  if (use_sem)
    semClose(semId);

  drawStringOnCursor((uint8_t *) "Succes", &hola);
  sleep(3000);
  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  initializeCursor(20,20, 2);
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = createProcessFromSched("my_process_inc", 0, 3, &my_process_inc,argvDec , 0);
    pids[i + TOTAL_PAIR_PROCESSES] = createProcessFromSched("my_process_dec", 0, 3, &my_process_inc ,argvInc, 0);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    waitProcessPid(pids[i]);
    waitProcessPid(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  return 0;
}