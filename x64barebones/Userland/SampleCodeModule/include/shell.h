#ifndef SHELL_H
#define SHELL_H

#define AMOUNT_OF_COMMANDS 16

// Shell manager
void shell();

// Checks if the command is valid
int interpretCommand(char * command);

// Executes the command
int executeCommand(char** arguments, int background, int pipePos, int argslen);

// Parses the command line to an array of arguments and marks if background and give pipe position
void parseCommand(char* commandLine, char** args, int* background, int *pipePos, int *argslen);

#endif