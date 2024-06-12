// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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