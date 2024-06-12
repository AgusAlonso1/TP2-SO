#ifndef LIB_C_H
#define LIB_C_H

#define DIM 610

int my_strlen(const char * s);
int strcmp(const char * s1, const char * s2);

int putString(char *c);
char getChar();
int printf(const char * format, ...);
char readChar(int * readBytes);
int randNbr(int fromIncluded, int toIncluded);
char getChar();

int scanf(const char * format, ...);

void itoa(int value, char* buff, int base);
void strcopy(char* destination, char* string);
int abs(int value);
void printChar(char c);
int wordlen(char* s);
int atoi(char* value);

void printChar(char c);

int fprintf(int fd, const char* string, ...);
int putStringFD(char * c, int fd);



#endif