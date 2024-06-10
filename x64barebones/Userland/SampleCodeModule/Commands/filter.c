#include <commands.h>

#define VOWELS_QTY 5

char vowels[] = {'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U'};

int isVowel(char c){
    for(int i = 0; i < VOWELS_QTY*2; i++) {
        if(c == vowels[i])
            return 1;
    }
    return 0;
}

int filter(int argc, char** argv) {
    char c; 
    while((c = getChar()) != EOF) {
        if(!isVowel(c)){
            printChar(c);
        }
    }
    return 0;
}