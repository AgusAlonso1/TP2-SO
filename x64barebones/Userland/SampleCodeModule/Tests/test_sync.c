#include <stdint.h>
#include "test_util.h"
#include <libc.h>
#include <syscallFunctions.h>


#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID 77

int64_t global; // shared memory
uint32_t hola;

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  call_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

int my_process_inc(int argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;
  uint64_t semId;

  if (argc != 4)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if ((semId = satoi(argv[3])) < 0)
     return -1;


  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      if(call_sem_wait(semId) == -1) printf("Error semwait\n");
    slowInc(&global, inc);
    if (use_sem)
      if(call_sem_post(semId) == -1) printf("Error sempost\n");
  }

  return 0;
}

int test_sync(int argc, char *argv[]) { //{n, use_sem, 0}

  int8_t useSem = satoi(argv[1]);

  uint32_t test_pid = call_get_pid();
  uint32_t pids[2 * TOTAL_PAIR_PROCESSES];

  uint64_t semId = call_get_new_sem_id();
  char semIdChar[30];
  itoa((int) semId, semIdChar, 10);

  if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], semIdChar,NULL};
  char *argvInc[] = {argv[0], "1", argv[1], semIdChar,NULL};

  global = 0;

   if (useSem)
    if ((call_sem_open(1, semId)) < 0) {
      printf("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  int fileDescriptors[CANT_FILE_DESCRIPTORS] = {DEV_NULL, STDOUT, STDERR};
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = call_create_process_background("my_process_inc", &my_process_inc, argvInc , test_pid, fileDescriptors);
    pids[i + TOTAL_PAIR_PROCESSES] = call_create_process_background("my_process_dec", &my_process_inc ,argvDec, test_pid, fileDescriptors);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    call_waitpid(pids[i]);
    call_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
  }


  printf("Succes %d\n", global);

  if (useSem)
		call_sem_close(SEM_ID);

  return 0;
}