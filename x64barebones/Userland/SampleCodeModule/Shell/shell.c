#include <shell.h>
#include <stdint.h>
#include <libc.h>
#include <commands.h>
#include <logo.h>
#include <themes.h>
#include <references.h>

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

int printShellHeader();

char * commands[AMOUNT_OF_COMMANDS] = {"man", "time", "registers", "snake", "div0", "invalidop", "clear", "zoomin", "zoomout", "settheme", "EstoesBoca"};
void (* commandsReferences[])() = {man, time, registers, snakeNewGame, div0, invalidOp, clear, zoomIn, zoomOut, theme, printBoca};

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
        scanf("%S", commandLine);

        parseCommand(commandLine, arguments, &background, &pipePos);

        if(*arguments[0] == 0){
            return;
        }

        int id = interpretCommand(arguments[0]);
        char flag = 0;
        executeCommand(id, &flag);

        if(flag == 0) {
            printf("Error: command not found\n");
        }
    }
}

int interpretCommand(char * command) {
    int index = -1;
    for (int i = 0; i < AMOUNT_OF_COMMANDS; i++) {
        if (strcmp(command, commands[i]) == 0) {
            return i;
        }
    }
    return index;
}

void executeCommand(int indexCommand, char * flag) {
    if (indexCommand == -1 ) {
        *flag = 0;
        return;
    }
    commandsReferences[indexCommand]();
    *flag = 1;
}

// Prints shell header and returns the y index of the corresponding header.
int printShellHeader() {
    uint32_t n;
    call_write((uint8_t *) "user> ", &n);
    call_c_get_y((int *)&n);
    return n;
}

void parseCommand(char* commandLine, char** args, int* background, int *pipePos){
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
            if (*cursor == '&') {
                *background = 1;
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
        args[i++] = token_start;
    }

    args[i] = NULL;
}

