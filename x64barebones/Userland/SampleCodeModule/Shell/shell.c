#include <shell.h>
#include <libc.h>
#include <commands.h>
#include <logo.h>
#include <themes.h>
#include <references.h>
#include <syscallFunctions.h>
#include <test_util.h>

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

char * commands[AMOUNT_OF_COMMANDS] = {"man", "time", "registers", "snake", "div0", "invalidop", "clear", "zoomin", "zoomout", "settheme", "EstoesBoca", "loop", "ps", "kill", "nice", "block", "minfo", "testmm", "testprocesses"};
int (* commandsReferences[AMOUNT_OF_COMMANDS])(int, char **) = {(int (*)(int, char **)) man,
                                                                (int (*)(int, char **)) time,
                                                                (int (*)(int, char **)) registers,
                                                                (int (*)(int, char **)) snakeNewGame,
                                                                (int (*)(int, char **)) div0,
                                                                (int (*)(int, char **)) invalidOp,
                                                                (int (*)(int, char **)) clear,
                                                                (int (*)(int, char **)) zoomIn,
                                                                (int (*)(int, char **)) zoomOut,
                                                                (int (*)(int, char **)) theme,
                                                                (int (*)(int, char **)) printBoca,
                                                                loop,
                                                                ps,
                                                                kill,
                                                                nice,
                                                                block,
                                                                minfo,
                                                                test_mm,
                                                                test_processes};

static char commandLine[BUFFER_SIZE] = {0};
static char *arguments[MAX_ARGUMENTS];

void shell() {
    defaultTheme();

    int cursorX = MIN_X, cursorY = MIN_Y, cursorScale = 1;
    call_c_init(cursorX, cursorY, cursorScale);

    while(1){
        printShellHeader();
        commandLine[0] = 0;
        int backgroundPos;
        int pipePos;
        int argslen;
        scanf("%S", commandLine);

        parseCommand(commandLine, arguments, &backgroundPos, &pipePos, &argslen);

        if(argslen == 0){
            return;
        }

        int flag = executeCommand(arguments, backgroundPos, pipePos, argslen);
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


int printShellHeader() {
    uint32_t n;
    call_write((uint8_t *) "user> ", &n);
    call_c_get_y((int *)&n);
    return n;
}

void parseCommand(char* commandline, char** args, int* backgroundPos, int *pipePos, int *argslen){
    *backgroundPos = -1;
    *pipePos = -1;

    int i = 0;
    char* token_start = commandline;
    char* cursor = commandline;

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
                *backgroundPos = i;
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


// si hay un pipe y no hay nada despues ---> ERROR
/*
int executeFunction(int indexCommand, int argc, char **argv) {
    if (indexCommand == -1 ) {
        return ERROR;
    }
    return commandsReferences[indexCommand](argc, argv);
}
 */

int executeCommand(char** args, int backgroundPos, int pipePos, int argslen){
    char * command1 = arguments[0];
    char * command2 = NULL;
    int flagBackground = 0;

    if(pipePos > 0){
        command2 = args[pipePos + 1];
    }

    if(backgroundPos > 0){
        flagBackground = 1;
    }


    int id1 = interpretCommand(command1);
    int id2 = interpretCommand(command2);

    uint32_t parentPid = (uint32_t) call_get_pid();

    int toReturn = ERROR;

    if(id1 != -1){
        char * arguments1[MAX_ARGUMENTS];
        int i, j;
        for(i = 1, j = 0; i < argslen && i != pipePos && i != backgroundPos; i++, j++){
            arguments1[j] = args[i];
        }
        arguments1[j] = NULL;

        if(id2 != -1){
            char * arguments2[MAX_ARGUMENTS];
            int k, m;
            for(k = pipePos + 2, m = 0; k < argslen && k != pipePos && i != backgroundPos; k++, m++){
                arguments2[m] = args[k];
            }
            arguments2[m] = NULL;
            toReturn = createPipedProcess(command1, arguments1, command2, arguments2, parentPid, flagBackground, id1, id2);
        } else {
            toReturn = createProcess(command1, arguments1, parentPid, flagBackground, id1);
        }
    }

    return toReturn;
}

int createPipedProcess(char* command1, char** arguments1, char* command2, char** arguments2, uint32_t parentPid, int background, int id1, int id2){
    int fileDescriptors1[CANT_FILE_DESCRIPTORS];
    int fileDescriptors2[CANT_FILE_DESCRIPTORS];

    int pipeId = call_get_pipe_id();

    fileDescriptors1[WRITE_FD] = pipeId;
    //  fileDescriptors1[ERROR_FD] = STDERR;

    fileDescriptors2[WRITE_FD] = STDOUT;
    fileDescriptors2[READ_FD] = pipeId;
    //  fileDescriptors2[ERROR_FD] = STDERR;

    uint32_t pid1;
    uint32_t pid2;
    if(background){
        fileDescriptors1[READ_FD] = DEV_NULL;
        pid1 = call_create_process_background(command1, (Function) commandsReferences[id1], arguments1, (uint32_t)parentPid,  fileDescriptors1);
        pid2 = call_create_process_background(command2, (Function) commandsReferences[id2], arguments2, (uint32_t)parentPid,  fileDescriptors2);
        if(pid1 == -1 || pid2 == -1){
            return ERROR;
        }
    } else {
        fileDescriptors1[READ_FD] = STDIN;
        pid1 = call_create_process_foreground(command1, (Function) commandsReferences[id1], arguments1, (uint32_t)parentPid,  fileDescriptors1);
        pid2 = call_create_process_foreground(command2, commandsReferences[id2], arguments2, (uint32_t)parentPid,  fileDescriptors2);
        if(pid1 != -1 && pid2 != -1){
            call_waitpid(pid1);
            call_waitpid(pid2);
        } else{
            return ERROR;
        }
    }
    return SUCCESS;
}

int createProcess(char* command1, char** arguments1, uint32_t parentPid, int background, int id1){
    int fileDescriptors[CANT_FILE_DESCRIPTORS];
    if(background){
        fileDescriptors[READ_FD] = DEV_NULL;
        fileDescriptors[WRITE_FD] = STDOUT;
      //  fileDescriptors[ERROR_FD] = STDERR;
        call_create_process_background(command1,(Function) commandsReferences[id1], arguments1, (uint32_t) parentPid,  fileDescriptors);
    } else {
        fileDescriptors[READ_FD] = STDIN;
        fileDescriptors[WRITE_FD] = STDOUT;
        //  fileDescriptors[ERROR_FD] = STDERR;
        uint32_t pid1 = call_create_process_foreground(command1,(Function) commandsReferences[id1], arguments1, (uint32_t) parentPid, fileDescriptors);

        if(pid1 != -1){
            call_waitpid(pid1);
        } else {
            return ERROR;
        }
    }
    return SUCCESS;
}

/*
int producer(){
    printf("Hola soy el que escribio...\n");
    return SUCCESS;
}

int consumer(){
    char* buf[30] = {0};
    scanf("%S", buf);   //imprime en pantalla lo que esta en buf
    return SUCCESS;
}
*/
