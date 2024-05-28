include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard */*.c)
SOURCES_ASM=$(wildcard */*.asm)
SOURCES_SHARED=$(wildcard ../Shared/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHARED=$(SOURCES_SHARED:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=_loader.c
LOADEROBJECT=$(LOADERSRC:.c=.o)

all: $(MODULE)

$(MODULE): $(OBJECTS) $(OBJECTS_ASM) $(LOADEROBJECT) $(OBJECTS_SHARED)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_SHARED) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(GCC) $(GCCFLAGS) $(LOADERSRC) -c -o $(LOADEROBJECT)

clean:
	rm -rf */*.o *.o *.bin ../Shared/*.o  # Se agregó la eliminación de archivos .o en Shared

.PHONY: all clean print
