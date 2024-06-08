include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard */*.c)
MODULEELF=0000-sampleCodeModule.elf
SOURCES_ASM=$(wildcard */*.asm)
SOURCES_SHARED=$(wildcard ../Shared/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHARED=$(SOURCES_SHARED:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=_loader.c
LOADEROBJECT=$(LOADERSRC:.c=.o)

all: $(MODULE)

$(MODULE):$(OBJECTS) $(OBJECTS_ASM) $(LOADEROBJECT)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_SHARED) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 -no-pie $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_SHARED) -o ../$(MODULEELF)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I../../Shared/include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(GCC) $(GCCFLAGS) -I./include -c $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf */*.o *.o *.bin ../Shared/*.o  # Se agregó la eliminación de archivos .o en Shared

.PHONY: all clean print
