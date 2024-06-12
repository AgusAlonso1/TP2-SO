// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idtManager.h>
#include <interruptions.h>

// El IDTR se inicia en la posisicon 0x0, con limite de 4096 bytes, 256 descriptores.

#pragma pack(push)
#pragma pack (1)

typedef struct {
    uint16_t offset_l;
    uint16_t selector;
    uint8_t cero;
    uint8_t access;
    uint16_t offset_m;
    uint32_t offset_h;
    uint32_t other_cero;
} IDTDescriptor;

#pragma pack(pop)

static void setUpIDTEntry(int index, uint64_t offset);

IDTDescriptor * idt = (IDTDescriptor *) 0; // Pointer to all descriptors.

void loadIDT() {
    setUpIDTEntry(0x00, (uint64_t) &_exception0Handler);    // Position 0x00
    setUpIDTEntry(0x06, (uint64_t) &_exception6Handler);    // Position 0x06
    setUpIDTEntry(0x20, (uint64_t) &_irq00Handler);         // Position 0x20
    setUpIDTEntry(0x21, (uint64_t) &_irq01Handler);         // Position 0x21
    setUpIDTEntry(0x80, (uint64_t) &_irq80Handler);         // Position 0x80

    picMasterMask(0xFC);
    picSlaveMask(0xFF);
}

void setUpIDTEntry(int index, uint64_t offset) {
    idt[index].offset_l = offset & 0xFFFF;
    idt[index].selector = 0x08;
    idt[index].cero = 0;
    idt[index].access = ACS_INT;
    idt[index].offset_m = (offset >> 16) & 0xFFFF;
    idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
    idt[index].other_cero = 0;
}
