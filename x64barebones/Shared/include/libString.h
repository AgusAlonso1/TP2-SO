#ifndef LIB_STRING_H
#define LIB_STRING_H
#include <stdint.h>

int my_strlen(const char * s);
void my_strcopy(char * destination, char* string);
uint64_t my_atoi(char *s);
int strcmp(const char * s1, const char * s2);
int stringArrayLen(char **args);
void itoa(int value, char* buff, int base);
void strcopy(char* destination, char* string);
int abs(int value);
void printChar(char c);
int wordlen(char* s);

uint8_t log2(uint64_t argument);
uint64_t pow2(uint64_t argument);


#endif