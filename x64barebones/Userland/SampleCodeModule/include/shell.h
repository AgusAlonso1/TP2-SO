#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

#define AMOUNT_OF_COMMANDS 22

#define KILL_INDEX 13
#define NICE_INDEX 14
#define BLOCK_INDEX 15

// Shell manager
void shell();

// Checks if the command is valid
int interpretCommand(char * command);

// Executes the shell function
int executeFunction(int indexCommand, int argc, char **argv);

// Executes the command
int executeCommand(char** arguments, int background, int pipePos, int argslen);

// Parses the command line to an array of arguments and marks if background and give pipe position
void parseCommand(char* commandLine, char** args, int* background, int *pipePos, int *argslen);

int createPipedProcess(char* command1, char** arguments1, char* command2, char** arguments2, uint32_t parentPid, int background, int id1, int id2);
int createProcess(char* command1, char** arguments1, uint32_t parentPid, int background, int id1);

int producer();

int consumer();



#endif