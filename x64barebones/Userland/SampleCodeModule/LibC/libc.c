// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <syscallFunctions.h>
#include <stdarg.h>

#define BUFFER_DIM 30
#define MAX_CHARS 150
#define BUFFER_SHELL_SIZE 256

#define UP_1 1
#define DOWN_1 4
#define LEFT_1 2
#define RIGHT_1 3


int strcmp(const char * s1, const char * s2) {
    int i = 0;
    while(s1[i] != 0  && s2[i] != 0){
        if(s1[i] != s2[i]){
            return s1[i] - s2[i];
        }
        i++;
    }
    return s1[i]-s2[i];
}


int my_strlen(const char * s) {
    int i = 0;
    while(s[i] != '\0') {
        if(s[i] == '\t') {
            i+=4;
        }
        i++;
    }
    return i;
}


int putString(char * c) {
    uint32_t length;
    call_write((int8_t *) c, &length, STDOUT);
    return length;
}

char getChar(){
    char c;
    uint32_t size = 0;
    while(size != 1){
        call_read(&c, 1, &size);
    }
    return c;
}

int printf(const char* string, ...){
    va_list v;

    char buffer[MAX_CHARS] = {0};
    char buffAux[25] = {0};     
    int i = 0, j = 0;               
    va_start(v, string);

    while(string && string[i]){
        if(string[i] == '%'){
            i++;
            switch(string[i]){
                case 'c':{
                    buffer[j] = va_arg(v, int);
                    j++;
                    break;
                }
                case 'd':{
                    itoa(va_arg(v, int), buffAux, 10);
                    strcopy(&buffer[j], buffAux);
                    j += my_strlen(buffAux);
                    break;
                }
                case 's':{
                    char* str = va_arg(v, char*);
                    strcopy(&buffer[j], str);
                    j += my_strlen(str);
                    break;
                }
                case 'x':{
                    itoa(va_arg(v, int), buffAux, 16);
                    strcopy(&buffer[j], buffAux);
                    j += my_strlen(buffAux);
                    break;
                }
                case 'p': {
                    uintptr_t num = (uintptr_t)va_arg(v, void*);
                    itoa(num, buffAux, 16);
                    for (int k = 0; buffAux[k] != '\0'; k++) {
                        buffer[j++] = buffAux[k];
                    }
                    break;
                }
            }
        }else{
            buffer[j++] = string[i];            
        }
        i++;
    }
    buffer[j] = 0;
    va_end(v);
    return putString(buffer);
}

char readChar(int * readBytes) {
    char buffer;
    call_read(&buffer, 1, (uint32_t *) readBytes);
    return buffer;
}

int randNbr(int fromIncluded, int toIncluded) {
    unsigned long long currentTicks;
    call_get_ticks(&currentTicks);
    return (fromIncluded + (currentTicks % (toIncluded)));
}

void printChar(char c){
    call_draw_char(c);
}


int scanf(const char* fmt, ...){
    va_list v;
    va_start(v, fmt);

    char c;
    char buffer[MAX_CHARS];
    int i = 0;

    while ((c = getChar()) != '\n' && i < BUFFER_SHELL_SIZE - 1){
        if(c == '\b' && i > 0){
            buffer[i--] = ' ';
            call_delete_char();
        }else if(c != '\b' && c != '\0'){
            buffer[i++] = c;
            printChar(c);
        }
    }
    putString("\n");

    buffer[i] = '\0';

    int params = 0;
    i = 0;
    int j = 0;

    while(fmt[j] != '\0' && buffer[i] != '\0' && i < MAX_CHARS){
        if(fmt[j] == '%'){
            j++;
            switch(fmt[j]){
                case 'c':{
                    *(char*) va_arg(v, char *) = buffer[i];
                    i++;
                    params++;
                    break;
                }
                case 'd':{
                    int aux = wordlen(&buffer[i]);
                    buffer[i+aux] = 0;
                    *(int*) va_arg(v, int *) = atoi(&buffer[i]);
                    buffer[i+aux] = ' ';
                    i += aux;
                    params++;
                    break;
                }
                case 's':{
                    int aux = wordlen(&buffer[i]);
                    buffer[i+aux] = '\0';
                    strcopy((char *) va_arg(v, char *), &buffer[i]);
                    buffer[i+aux] = ' ';
                    i += aux;
                    params++;
                    break;
                }
                case 'S':{
                    int aux = my_strlen(&buffer[i]);
                    buffer[i+aux] = '\0';
                    strcopy((char *) va_arg(v, char *), &buffer[i]);
                    buffer[i+aux] = ' ';
                    i += aux;
                    params++;
                    break;
                }
            }
        } else if (fmt[j] == buffer[i]){
            i++;
        } else{
            putString("ERROR");
        }
        j++;
    }

    va_end(v);
    return params;
}



void itoa(int value, char* buff, int base){
    int i = 0;
    int val = abs(value);

    if(val == 0){
        buff[i++] = '0';
    }

    while(val != 0){
        int resto = val % base;
        if(resto < 10){
            buff[i++] = resto + '0';
        }else {
            buff[i++] = resto + 'A' - 10;
        }
        val /= base;
    }

    if (value < 0 && base == 10) {
        buff[i++] = '-';
    }
    int dim = i;

    int j = 0;
    i -= 1;
    char aux;
    while(j < i){
        aux = buff[j];
        buff[j] = buff[i];
        buff[i] = aux;
        j++;
        i--;
    }
    buff[dim] = 0;

}

void strcopy(char* destination, char* string){
    if(destination == 0){
        return;
    }
    int i;
    for(i  = 0; string[i] != '\0'; i++){
        destination[i] = string[i];
    }
    destination[i] = '\0';
}

int abs(int value){
    if(value < 0){
        return -value;
    }
    return value;
}

int atoi(char* value){
    int isNegative = 0;
    int aux = 0;
    if(value[0] == '-'){
        isNegative = 1;
    }
    for (int i = isNegative; value[i] != '\0'; ++i){
        if(value[i] >= '0' && value[i] <= '9'){
            aux = aux * 10 + value[i] - '0';
        }
    }

    if(isNegative){
        aux = -aux;
    }
    return aux;
}

int wordlen(char* s){
    int aux = 0;
    while(s[aux] != 0 && s[aux] != ' '){
        aux++;
    }
    return aux;
}


int fprintf(int fd, const char* string, ...){
    va_list v;

    char buffer[MAX_CHARS] = {0};
    char buffAux[25] = {0};
    int i = 0, j = 0;
    va_start(v, string);

    while(string && string[i]){
        if(string[i] == '%'){
            i++;
            switch(string[i]){
                case 'c':{
                    buffer[j] = va_arg(v, int);
                    j++;
                    break;
                }
                case 'd':{
                    itoa(va_arg(v, int), buffAux, 10);
                    strcopy(&buffer[j], buffAux);
                    j += my_strlen(buffAux);
                    break;
                }
                case 's':{
                    char* str = va_arg(v, char*);
                    strcopy(&buffer[j], str);
                    j += my_strlen(str);
                    break;
                }
                case 'x':{
                    itoa(va_arg(v, int), buffAux, 16);
                    strcopy(&buffer[j], buffAux);
                    j += my_strlen(buffAux);
                    break;
                }
                case 'p': {
                    uintptr_t num = (uintptr_t)va_arg(v, void*);
                    itoa(num, buffAux, 16);
                    for (int k = 0; buffAux[k] != '\0'; k++) {
                        buffer[j++] = buffAux[k];
                    }
                    break;
                }
            }
        }else{
            buffer[j++] = string[i];
        }
        i++;
    }
    buffer[j] = 0;      // asi le indico que aca terminamos
    va_end(v);
    return putStringFD(buffer, fd);
}

int putStringFD(char * c, int fd) {
    uint32_t length;
    call_write((int8_t *) c, &length, fd);
    return length;
}