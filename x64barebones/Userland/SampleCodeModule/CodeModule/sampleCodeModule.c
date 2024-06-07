/* sampleCodeModule.c */
#include <syscallFunctions.h>
#include <shell.h>
#include <libc.h>
#include <themes.h>
#include <references.h>

#define CHARACTER_COLOR 0xB0CA07
#define TAB_SIZE 4
#define HEADER_SIZE 6
#define MIN_X 8
#define MIN_Y 8
#define MAX_X 1008
#define MAX_Y 1008
#define WIDTH_FONT 8
#define HEIGHT_FONT 16
#define BUFFER_SIZE 256

int printShellHeader();
void exceptionHandler(uint64_t exceptionNumber, char * errorMessage);
void terminal();

static reference shellReference;

int main(void) {
    call_set_exception_handler(0, exceptionHandler);
    call_set_exception_handler(6, exceptionHandler);

    saveReference(&shellReference);
    shell();

    return 0;
}

void exceptionHandler(uint64_t exceptionNumber, char * errorMessage) {
    setTheme(6);
    call_c_init(MIN_X, MIN_Y, 1);
    putString("\t\t\t\t\t\t\t\t\t\t\t\t");
    switch(exceptionNumber) {
        case 0 : // Division by Zero
            putString(errorMessage);
            break;
        case 6 : // Invalid Opcode
            putString(errorMessage);
            break;
        default :
            putString("ERROR - Unknown Exception");
            break;
    }

    call_get_registers();

    char c = 0;

    printf("\n\n\n\t\t\t\t\t\t\t\t\t\t\t\tPress \"ENTER\" key to continue..\n");

    do {
        c = getChar();
    } while(c == 0);

    jumpToReference(&shellReference);
}