// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtManager.h>
#include <videoDriver.h>
#include <processes.h>
#include <interruptions.h>
#include <scheduler.h>
#include <timer.h>
#include <pipeMaster.h>
#include <semaphores.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;


typedef int (*EntryPoint)();



void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	void * moduleAddresses[] = {sampleCodeModuleAddress,sampleDataModuleAddress };
    loadModules(&endOfKernelBinary, moduleAddresses);
    clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

int idle(int argc, char **argv){
    char * args[] = {"2", "shell", NULL};   // el 2 es el argc
    uint32_t currentPid = getCurrentPid();
    int fileDescriptors[CANT_FILE_DESCRIPTORS] = {STDIN, STDOUT, STDERR};
    createProcessFromSched("shell", 1, LEVEL4, (Function) sampleCodeModuleAddress, args, currentPid, 1, fileDescriptors);

    while (1){
        _hlt();
    }

    return 0;
}


int main() {
	_cli();
	createMemoryManager((void * ) MEMORY_MANAGER_FIRST_ADDRESS, pow2(MAX_EXP));
	createScheduler();
    createPipeMaster();
    createSemaphoreManager();

	char * args[] = {"2", "idle", NULL};
    int fileDescriptors[CANT_FILE_DESCRIPTORS] = {STDIN, STDOUT, STDERR};
    createProcessFromSched("idle", 0, LEVEL3, (Function) &idle, args, IDLE, 1, fileDescriptors);

    loadIDT();
    _sti();
    return 0;
}