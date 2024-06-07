#include <stddef.h>
#include <libString.h>

int stringArrayLen(char **args) {
    int len = 0;
    while (args != NULL && args[len] != NULL) {
        len++;
    }
    return len;
}

uint64_t my_atoi(char *s) {
    uint64_t acum = 0;
    uint64_t factor = 1;

    if(*s == '-') {
        factor = -1;
        s++;
    }

    while((*s >= '0')&&(*s <= '9')) {
        acum = acum * 10;
        acum = acum + (*s - 48);
        s++;
    }
    return (factor * acum);
}

void my_strcopy(char* destination, char* string) {
    if(destination == 0){
        return;
    }

    int i;
    for(i  = 0; string[i] != '\0'; i++){
        destination[i] = string[i];
    }
    destination[i] = '\0';
}

//esta desps ponerla en algun otro lugar, no aca
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

void itoa(int value, char* buff, int base){
    int i = 0;
    int val = abs(value);       // tomo el valor absoluto para no mandarme un moco con los signos

    if(val == 0){
        buff[i++] = '0';
    }

    while(val != 0){
        int resto = val % base;
        if(resto < 10){
            buff[i++] = resto + '0';
        }else{                          // con esto vemos si es hexa
            buff[i++] = resto + 'A' - 10;
        }
        val /= base;
    }

    if (value < 0 && base == 10) {      // si es base 10, le pongo el signo menos
        buff[i++] = '-';
    }
    int dim = i;        // me guardo dim del numero

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

static int strConcat(char *str1, char *str2){
    int i = my_strlen(str1);
    int j = 0;
    while(str2[j] != '\0'){
        str1[i] = str2[j];
        i++;
        j++;
    }
    return i;
}

