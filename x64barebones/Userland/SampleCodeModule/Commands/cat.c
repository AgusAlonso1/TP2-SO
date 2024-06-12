// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>

int cat(int argc, char** argv) {
    char c;
    while ((c = getChar()) != EOF) {
        printChar(c);
    }
    return 0;
}