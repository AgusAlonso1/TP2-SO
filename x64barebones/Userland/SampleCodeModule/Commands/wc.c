#include <commands.h>

int wc(int argc, char** argv) {
    char newWord = 0;
    char c;
    unsigned int amountOfChars = 0, amountOfWords = 0, amountOfLines= 0;
    while ((c = getChar())!= EOF/* ACA VA EL EOF*/) {
        if (c == ' ' && newWord) {
            amountOfWords++;
            newWord = 0;
        }
        if (c == '\n') {
            amountOfWords++;
            amountOfLines++;
            newWord = 0;
        }
        amountOfChars++;
    }

    printf("Chars: %d, Words: %d, Lines: %d \n", amountOfChars, amountOfWords, amountOfLines);
    return 0;
}