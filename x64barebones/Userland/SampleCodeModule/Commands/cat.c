#include <commands.h>

int cat(int argc, char** argv) {
    char c;
    while ((c = getChar()) != 0/* ACA VA EL EOF*/) {
        printChar(c);
    }
    return 0;
}