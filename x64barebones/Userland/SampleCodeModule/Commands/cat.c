#include <commands.h>

int cat(int argc, char** argv) {
    char c;
    while ((c = getChar()) != EOF/* ACA VA EL EOF*/) {
        printChar(c);
    }
    return 0;
}