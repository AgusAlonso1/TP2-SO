#include <commands.h>

int wc(int argc, char** argv) {
    char newWord = 1;
    char c;
    unsigned int amountOfChars = 0, amountOfWords = 0, amountOfLines= 0;

    while ((c = getChar()) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (!newWord) {
                amountOfWords++;
                newWord = 1;
            }
            if (c == '\n') {
                amountOfLines++;
            }
        } else {
            newWord = 0;
        }
        amountOfChars++;
    }

    if (!newWord) {
        amountOfWords++;
    }

    printf("Chars: %d, Words: %d, Lines: %d \n", amountOfChars, amountOfWords, amountOfLines);
    return 0;
}
