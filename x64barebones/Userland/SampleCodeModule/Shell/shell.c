#include <shell.h>
#include <stdint.h>
#include <libc.h>
#include <commands.h>
#include <logo.h>
#include <themes.h>
#include <references.h>
#include <syscallFunctions.h>

#define CHARACTER_COLOR 0xB0CA07
#define TAB_SIZE 4
#define HEADER_SIZE 6
#define MIN_X 8
#define MIN_Y 8
#define MAX_X 1008
#define MAX_Y 1008
#define WIDTH_FONT 8
#define HEIGHT_FONT 16
#define MAX_ARGUMENTS 7
#define BUFFER_SIZE 256
#define ERROR 0
#define SUCCESS 1

int printShellHeader();

char * commands[AMOUNT_OF_COMMANDS] = {"man", "time", "registers", "snake", "div0", "invalidop", "clear", "zoomin", "zoomout", "settheme", "EstoesBoca", "loop"};
void (* commandsReferences[])() = {man, time, registers, snakeNewGame, div0, invalidOp, clear, zoomIn, zoomOut, theme, printBoca, loop};

static char commandLine[BUFFER_SIZE] = {0};
static char *arguments[MAX_ARGUMENTS];

void shell() {
    defaultTheme();

    int cursorX = MIN_X, cursorY = MIN_Y, cursorScale = 1;
    call_c_init(cursorX, cursorY, cursorScale);

    while(1){
        printShellHeader();
        commandLine[0] = 0;
        int background;
        int pipePos;
        int argslen;
        scanf("%S", commandLine);

        parseCommand(commandLine, arguments, &background, &pipePos, &argslen);

        if(argslen == 0){
            return;
        }

        /*
        int id = interpretCommand(arguments[0]);
        char flag = 0;
        executeCommand(id, &flag);
        */

        char flag = executeCommand(arguments, background, pipePos, argslen);
        if(flag == ERROR) {
            printf("Error: command not found\n");
        }
    }
}

int interpretCommand(char * command) {
    int index = -1;
    for (int i = 0; i < AMOUNT_OF_COMMANDS && command != NULL; i++) {
        if (strcmp(command, commands[i]) == 0) {
            return i;
        }
    }
    return index;
}

/*
void executeCommand(int indexCommand, char * flag) {
    if (indexCommand == -1 ) {
        *flag = 0;
        return;
    }
    commandsReferences[indexCommand]();
    *flag = 1;
}
*/

// Prints shell header and returns the y index of the corresponding header.
int printShellHeader() {
    uint32_t n;
    call_write((uint8_t *) "user> ", &n);
    call_c_get_y((int *)&n);
    return n;
}

void parseCommand(char* commandLine, char** args, int* background, int *pipePos, int *argslen){
    *background = 0;
    *pipePos = -1;

    int i = 0;
    char* token_start = commandLine;
    char* cursor = commandLine;

    while (*cursor != '\0') {
        if (*cursor == '|' || *cursor == '&') {
            if (cursor != token_start) {
                args[i++] = token_start;
                *cursor = '\0';
                token_start = cursor + 1;
            }
            if (*cursor == '|') {
                *pipePos = i;
            }
            if (*cursor == '&') {      //donde encuentra este, ahi se termina el comando
                *background = 1;
                cursor++;
                break;
            }
        } else if (*cursor == ' ' || *cursor == '\t') {
            if (cursor != token_start) {
                args[i++] = token_start;
                *cursor = '\0';
                token_start = cursor + 1;
            }
        }
        cursor++;
    }

    if (cursor != token_start) {
        args[i] = token_start;
        i++;
    }

    args[i++] = NULL;
    *argslen = i;
}

int executeCommand(char** arguments, int background, int pipePos, int argslen){
    char * command1 = arguments[0];
    char * command2 = NULL;

    if(pipePos != -1){
        command2 = arguments[pipePos + 1];
    }

    int id1 = interpretCommand(command1);
    int id2 = interpretCommand(command2);

    uint32_t parentPid = (uint32_t) call_get_pid();

    if(id1 != -1){
        char * arguments1[MAX_ARGUMENTS];
        uint32_t  pid1;
        int i, j;
        for(i = 1, j = 0; i < argslen && i != pipePos; i++, j++){
            arguments1[j] = arguments[i];
        }
        arguments1[j] = NULL;
        if(id2 != -1){
            char * arguments2[MAX_ARGUMENTS];
            for(int i = 1, j = 0; i < argslen && i != pipePos; i++, j++){
                arguments2[j] = arguments[i];
            }
            arguments2[j] = NULL;
            //aca falta logica de file descriptor
            pid1 = call_create_process(command1, 0, (Function) commandsReferences[id1], arguments1, parentPid);
            uint32_t  pid2 = call_create_process(command2, !background, (Function) commandsReferences[id2], arguments2, parentPid);
            if(!background && pid1 != -1 && pid2 != -1){
                call_waitpid(pid1);
                call_waitpid(pid2);
            } else{
                return ERROR;
            }
        } else {
            printf("command1: %s\n", command1);
            printf("arguments1: %s\n", arguments1[0]);
            printf("background: %c\n", background);
            pid1 = call_create_process(command1, !background,(Function) commandsReferences[id1], arguments1, parentPid);
            if(!background && pid1 != -1){
                call_waitpid(pid1);
            } else{
                return ERROR;
            }
        }
    } else {
        return ERROR;
    }

    return SUCCESS;
}

