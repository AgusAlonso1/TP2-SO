#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stddef.h>

uint64_t syscallsDispatcher(uint64_t rax, uint64_t *otherRegisters);

#endif
