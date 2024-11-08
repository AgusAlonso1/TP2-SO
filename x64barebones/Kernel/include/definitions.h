#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <memoryManager.h>

// Access
#define ACS_PRESENT 0x80        // Present segment
#define ACS_CSEG 0x18           // Code segment
#define ACS_DSEG 0x20           // Data segment
#define ACS_READ 0x02           // Read segment
#define ACS_WRITE 0x02          // Write segment
#define ACS_IDT ACS_CSEG        // Interrupt Descriptor Table
#define ACS_INT_386 0x0E	    // Interrupt GATE 32 bits
#define ACS_INT ( ACS_PRESENT | ACS_INT_386 )

#define ACS_CODE (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#define MEMORY_MANAGER_ADDRESS (void *) 0x500000
#define MEMORY_INFO_ADDRESS 0x580000
#define SCHEDULER_ADDRESS 0x510000
#define PIPE_MASTER_ADDRESS 0x520000
#define SEMAPHORES_MANAGER_ADDRESS 0x530000

#define MEMORY_MANAGER_FIRST_ADDRESS 0x600000

#endif
