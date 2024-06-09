#include "syscall.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memoryManager.h>
#include <cursor.h>
#include <lib.h>
#include <timer.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = allocMemory(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address){
          while( mm_rqs[i].address < (void *) 0x500000){
              ;
          }
          memset(mm_rqs[i].address, i, mm_rqs[i].size);
      }


    initializeCursor(20, 20, 2);
    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
            uint32_t hola = 20;
            drawStringOnCursor((uint8_t *)"ERROR!", &hola);
          //F("test_mm ERROR\n");
          return -1;
        }

    uint32_t chau = 20;
    drawStringOnCursor((uint8_t *)"OK!", &chau);
    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        freeMemory(mm_rqs[i].address);
    break;
  }
  MemoryManagerADT mm = getMemoryManager();
}