#include <syscalls.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <time.h>
#include <exceptions.h>
#include <cursor.h>
#include <timer.h>
#include <interruptions.h>
#include <sound.h>
#include <memoryManager.h>
#include <pipeMaster.h>
#include <memoryasm.h>
#include <scheduler.h>
#include <semaphores.h>



typedef enum {SYS_READ = 0, SYS_WRITE, DRAW_C, DELETE_C, TIME, THEME, SET_EXC, C_GET_X, C_GET_Y, C_GET_S, C_SET_S, C_MOVE, C_INIT, SET_COLORS, GET_REGS, DRAW_SQUARE, COLOR_SCREEN, DRAW_CIRCLE, CLEAR_SCREEN, SLEEP, GET_TICKS, BEEP, MALLOC, FREE_MEMORY, CREATE_PROCESS_FOREGROUND, KILL_PROCESS, GET_PROCESSES_COPY, GET_PID, GET_PARENT_PID, SET_PRIORITY, BLOCK, WAITPID, FREE_PROCESS_COPY, CREATE_PROCESS_BACKGROUND, GET_PIPE_ID, PIPE_OPEN, PIPE_CLOSE, PIPE_WRITE, PIPE_READ, GET_MEM_INFO, SEM_OPEN, SEM_CLOSE, SEM_WAIT, SEM_POST, YIELD, SLEEP_SECONDS, GET_NEW_SEM_ID}SysID;

static void sys_read(char * buf, uint32_t count, uint32_t * readBytes);
//static void sys_write(uint8_t * buf, uint32_t x, uint32_t y, uint32_t scale, uint32_t * count);
static void sys_write(int8_t * buf, uint32_t * count, int userlandFd);
//static void sys_draw_char(uint8_t character, uint32_t x, uint32_t y, uint32_t scale);
static void sys_draw_char(uint8_t character);
//static void sys_delete_char( uint32_t x, uint32_t y, uint32_t scale) ;
static void sys_delete_char();
static void sys_shell_theme(uint32_t * theme);
static void sys_time(uint8_t ** currentTime);
static void sys_set_exception_handler(uint64_t number, exceptionHandler exception);
static void sys_get_cursor_x(int * x);
static void sys_get_cursor_y(int * y);
static void sys_get_cursor_scale(int * scale);
static void sys_set_cursor_scale(int scale);
static void sys_move_cursor(actionOfCursor action);
static void sys_init_cursor(int x, int y, int scale);
static void sys_set_colors(uint32_t textColor, uint32_t backgroundColor);
static void sys_get_registers();
static void sys_draw_square(uint32_t hexColor,uint64_t x, uint64_t y, uint32_t scale);
static void sys_color_screen(uint32_t hexColor);
static void sys_draw_circle(uint32_t color, uint32_t x, uint32_t y, uint32_t length, uint32_t backgroundColor);
static void sys_clear_screen();
static void sys_sleep(unsigned long long ms);
static void sys_get_ticks(unsigned long long * ticks);
static void sys_beep(uint32_t frequency);
static void * sys_malloc(uint64_t size);
static void sys_free(void * ptrToFree);
static int32_t sys_create_process_foreground(char* name, Function function, char **args, uint32_t parentPid, const int fileDescriptors[CANT_FILE_DESCRIPTORS]);
static int64_t sys_kill_process(uint32_t pid);
static ProcessCopyList * sys_get_processes_copy();
static uint32_t sys_get_pid();
static uint32_t sys_get_parent_pid();
static int64_t sys_set_priority(uint32_t pid, uint64_t priority);
static int64_t sys_block(uint32_t pid);
static int sys_waitpid(uint32_t pid);
static void sys_free_process_copy(ProcessCopyList * processCopyList);
static int32_t sys_create_process_background(char* name, Function function, char **args, uint32_t parentPid, const int fileDescriptors[CANT_FILE_DESCRIPTORS]);
static int sys_get_pipe_id();
static int16_t sys_pipe_open(int id, char mode);
static int16_t sys_pipe_close(int id);
static int16_t sys_pipe_write(int id, char* msg, int len);
static int16_t sys_pipe_read(int id, char* buffer, int len, uint32_t * readBytes);
static uint64_t sys_get_mem_info();
static int64_t sys_sem_open(uint64_t value, uint64_t semId);
static int8_t sys_sem_close(uint64_t semId);
static int64_t sys_sem_wait(uint64_t semId);
static int64_t sys_sem_post(uint64_t semId);
static void sys_yield();
static void sys_sleep_seconds(unsigned long long seconds);
static uint64_t sys_get_new_sem_id();



uint64_t syscallsDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t aux) {
    switch(rdi) {
        case SYS_READ :
            sys_read((char *) rsi, (uint32_t) rdx, (uint32_t *) rcx);
            break;
        case SYS_WRITE :
            sys_write((int8_t *) rsi, (uint32_t *) rdx, (int) rcx);
            break;
        case DRAW_C :
            sys_draw_char((uint8_t) rsi);
            break;
        case DELETE_C :
            sys_delete_char();
            break;
        case TIME :
            sys_time((uint8_t **) rsi);
            break;
        case THEME :
            sys_shell_theme((uint32_t *) rsi);
            break;
        case SET_EXC :
            sys_set_exception_handler((uint64_t) rsi, (exceptionHandler) rdx);
            break;
        case C_GET_X :
            sys_get_cursor_x((int *) rsi);
            break;
        case C_GET_Y :
            sys_get_cursor_y((int *) rsi);
            break;
        case C_GET_S :
            sys_get_cursor_scale((int *) rsi);
            break;
        case C_SET_S :
            sys_set_cursor_scale((int) rsi);
            break;
        case C_MOVE :
            sys_move_cursor((actionOfCursor) rsi);
            break;
        case C_INIT :
            sys_init_cursor((int) rsi, (int) rdx, (int) rcx);
            break;
        case SET_COLORS :
            sys_set_colors((uint32_t) rsi, (uint32_t) rdx);
            break;
        case GET_REGS :
            sys_get_registers();
            break;
        case DRAW_SQUARE:
            sys_draw_square((uint32_t) rsi, (uint32_t) rdx, (uint32_t) rcx, (uint32_t) r8);
            break;
        case COLOR_SCREEN :
            sys_color_screen((uint32_t) rsi);
            break;
        case DRAW_CIRCLE :
            sys_draw_circle((uint32_t) rsi, (uint32_t) rdx, (uint32_t) rcx, (uint32_t) r8, (uint32_t) r9);
            break;
        case CLEAR_SCREEN :
            sys_clear_screen();
            break;
        case SLEEP :
            sys_sleep((unsigned long long) rsi);
            break;
        case GET_TICKS :
            sys_get_ticks((unsigned long long *) rsi);
            break;
        case BEEP :
            sys_beep((uint32_t) rsi);
            break;
        case MALLOC:
            return (uint64_t) sys_malloc((uint64_t) rsi);
        case FREE_MEMORY:
            sys_free((void *) rsi);
            break;
        case CREATE_PROCESS_FOREGROUND:
            return (uint64_t) sys_create_process_foreground((char *) rsi, (Function) rdx, (char **) rcx, (uint32_t) r8,(const int *) r9);
        case KILL_PROCESS:
            return sys_kill_process((uint32_t) rsi);
        case GET_PROCESSES_COPY:
            return (uint64_t) sys_get_processes_copy();
        case GET_PID:
            return (uint64_t) sys_get_pid();
        case GET_PARENT_PID:
            return (uint64_t) sys_get_parent_pid();
        case SET_PRIORITY:
            return sys_set_priority((uint32_t) rsi, (uint64_t) rdx);
        case BLOCK:
            return sys_block((uint32_t) rsi);
        case WAITPID:
            return sys_waitpid((uint32_t) rsi);
        case FREE_PROCESS_COPY:
            sys_free_process_copy((ProcessCopyList *) rsi);
            break;
        case CREATE_PROCESS_BACKGROUND:
            return (uint64_t) sys_create_process_background((char *) rsi, (Function) rdx, (char **) rcx, (uint32_t) r8,(const int *) r9);
        case GET_PIPE_ID:
            return sys_get_pipe_id();
        case PIPE_OPEN:
            return sys_pipe_open((int) rsi, (char) rdx);
        case PIPE_CLOSE:
            return sys_pipe_close((int) rsi);
        case PIPE_WRITE:
            return sys_pipe_write((int) rsi, (char *) rdx, (int) rcx);
        case PIPE_READ:
            return sys_pipe_read((int) rsi, (char *) rdx, (int) rcx, (uint32_t *) r8);
        case GET_MEM_INFO:
            return sys_get_mem_info();
            break;
        case SEM_OPEN:
            return sys_sem_open(rsi, rdx);
        case SEM_CLOSE:
            return sys_sem_close(rsi);
        case SEM_WAIT:
            return sys_sem_wait(rsi);
        case SEM_POST:
            return sys_sem_post(rsi);
        case YIELD:
            sys_yield();
            break;
        case SLEEP_SECONDS:
            sys_sleep_seconds((uint64_t) rsi);
            break;
        case GET_NEW_SEM_ID:
            return sys_get_new_sem_id();
        default :
            break;
    }
    return -1;
}

// Syscall Read - ID = 0
static void sys_read(char * buf, uint32_t count, uint32_t * readBytes) {
    uint32_t pid = getCurrentPid();
    int fd = getCurrentReadFileDescriptor();

    if(fd == STDIN){
        readFromKeyboard(buf, count, readBytes);
    } else if(fd != DEV_NULL) {
        pipeRead(fd, pid,buf, (int) count, readBytes);
    }
}

// Syscall Write - ID = 1
static void sys_write(int8_t * buf, uint32_t * count, int userlandFd) {
    uint32_t pid = getCurrentPid();
    int fd = getCurrentWriteFileDescriptor();

    if(userlandFd == STDERR){
        uint32_t oldLetterColor = getFontColor();
        uint32_t errorColor = 0xFF0000;
        setFontColor(errorColor);
        drawStringOnCursor(buf, count); //con color ROJO
        setFontColor(oldLetterColor);
    }else{
        if(fd == STDOUT){
            drawStringOnCursor(buf, count);
        } else if(fd != DEV_NULL){
            int len = my_strlen((char *) buf);
            pipeWrite(fd, pid, (char *) buf, len);
        }
    }
}

// Syscall Draw char - ID = 2
static void sys_draw_char(uint8_t character) {
    drawCharOnCursor(character);
}

// Syscall Delete char - ID = 3
static void sys_delete_char() {
    deleteCharOnCursor();
}

//Syscall Time - ID = 4
static void sys_time(uint8_t ** currentTime) {
    *currentTime = get_time();
}

// Syscall theme  - ID = 5
static void sys_shell_theme(uint32_t * theme) {
    uint32_t font_color = theme[1], background_color = theme[0];
    setColor(font_color, background_color);
    sys_color_screen(background_color);
}

static void sys_set_exception_handler(uint64_t number, exceptionHandler exception) {
    setExceptionHandler(number, exception);
}

static void sys_get_cursor_x(int * x) {
    *x = getCursorX();
}


static void sys_get_cursor_y(int * y) {
    *y = getCursorY();
}

static void sys_get_cursor_scale(int * scale) {
    *scale = getCursorScale();
}

static void sys_set_cursor_scale(int scale) {
    setCursorScale(scale);
}

static void sys_move_cursor(actionOfCursor action) {
    moveCursor(action);
}

static void sys_init_cursor(int x, int y, int scale) {
    initializeCursor(x, y, scale);
}

static void sys_set_colors(uint32_t textColor, uint32_t backgroundColor) {
    setColor(textColor,backgroundColor);
}

static void sys_get_registers(){
    if(!savedRegs()){
        uint32_t length;
        sys_write((int8_t *)"Registers must be saved.\n", &length, STDERR);
        return;
    }
    uint32_t length;
    char toHex[18];
    for(int i = REGISTERS_AMOUNT-1; i >= 0; i--) {
        copyRegisters(getRegisterValue(i), toHex);
        sys_write((int8_t *)getRegisterName(i), &length, STDOUT);
        sys_write((int8_t *) toHex, &length, STDOUT);
        sys_write((int8_t *)"\n", &length, STDOUT);
    }
}

static void sys_draw_square(uint32_t hexColor,uint64_t x, uint64_t y, uint32_t scale) { 
    drawSquare(hexColor,x,y,scale);
}

static void sys_color_screen(uint32_t hexColor) {
    colorScreen(hexColor);
}

static void sys_draw_circle(uint32_t color, uint32_t x, uint32_t y, uint32_t length, uint32_t backgroundColor) {
    drawCircle(color, x, y, length, backgroundColor);
}

static void sys_clear_screen() {
    colorScreen(getBackgroundColor());
    int scale;
    sys_get_cursor_scale(&scale);
    sys_init_cursor(MIN_X, MIN_Y, scale);
}

static void sys_sleep(unsigned long long ms) {
    sleep(ms);
}

static void sys_get_ticks(unsigned long long * ticks) {
    *ticks = ticks_elapsed();
}

static void sys_beep(uint32_t frequency) {
    beep(frequency);
}

static void * sys_malloc(uint64_t size) {
    return allocMemory(size);
}

static void sys_free(void * ptrToFree) {
    return freeMemory(ptrToFree);
}

static int32_t sys_create_process_foreground(char* name, Function function, char **args, uint32_t parentPid, const int fileDescriptors[CANT_FILE_DESCRIPTORS]){
    return createProcessFromSched((char*) name, (char) FOREGROUND, LEVEL3, (Function) function,(char**) args, (uint32_t)parentPid, 0, fileDescriptors);
}

static int64_t sys_kill_process(uint32_t pid){
    return killProcess(pid);
}

static ProcessCopyList * sys_get_processes_copy() {
    return getProcessCopy();
}

static uint32_t sys_get_pid() {
    return getCurrentPid();
}

static uint32_t sys_get_parent_pid() {
    return getCurrentParentPid();
}

static int64_t sys_set_priority(uint32_t pid, uint64_t priority){
    return setPriority(pid, priority);
}

static int64_t sys_block(uint32_t pid){
    return block(pid);
}

static int sys_waitpid(uint32_t pid){
    return waitProcessPid(pid);
}

static void sys_free_process_copy(ProcessCopyList * processCopyList) {
    freeProcessCopy(processCopyList);
}

static int32_t sys_create_process_background(char* name, Function function, char **args, uint32_t parentPid, const int fileDescriptors[CANT_FILE_DESCRIPTORS]) {
    return createProcessFromSched((char*) name, (char) BACKGROUND, LEVEL3, (Function) function,(char**) args, (uint32_t)parentPid, 0, fileDescriptors);
}

static int sys_get_pipe_id(){
    return getPipeId();
}

static int16_t sys_pipe_open(int id, char mode) {
    return pipeOpen(id, mode);
}

static int16_t sys_pipe_close(int id) {
    return pipeClose(id);
}

static int16_t sys_pipe_write(int id, char* msg, int len) {
    return pipeWrite(id, getCurrentPid(), msg, len);
}

static int16_t sys_pipe_read(int id, char* buffer, int len, uint32_t * readBytes) {
    return pipeRead(id, getCurrentPid(), buffer, len, readBytes);
}

static uint64_t sys_get_mem_info() {
    return 1;
    // Me falta merge
}

static int64_t sys_sem_open(uint64_t value, uint64_t semId) {
    return semOpen(value, semId);
}

static int8_t sys_sem_close(uint64_t semId) {
    return semClose(semId);
}

static int64_t sys_sem_wait(uint64_t semId) {
    return semWait(semId);
}

static int64_t sys_sem_post(uint64_t semId) {
    return semPost(semId);
}

static void sys_yield(){
    yield();
}

static void sys_sleep_seconds(unsigned long long seconds) {
    sleepSeconds(seconds);
}

static uint64_t sys_get_new_sem_id(){
    return getNewSemId();
}