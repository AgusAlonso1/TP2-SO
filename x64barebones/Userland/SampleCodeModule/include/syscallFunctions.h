#ifndef SYSCALL_FUNCTIONS_H
#define SYSCALL_FUNCTIONS_H

#include <stddef.h>
#include <stdint.h>
#include <globals.h>


typedef enum {WRITE=0, DELETE, ENTER, RELATIVE_ENTER, TAB}actionOfCursor;

void call_read(char * buf, uint32_t count, uint32_t * readBytes);
void call_write(int8_t * buf, uint32_t * length);
void call_draw_char(uint8_t character);
void call_delete_char();
void call_get_time(uint8_t ** time);
void call_set_theme(uint32_t * theme);
void call_set_exception_handler(uint64_t number, void * exception);
void call_c_get_x(int * x);
void call_c_get_y(int * y);
void call_c_get_scale(int * scale);
void call_c_set_scale(int scale);
void call_c_move(actionOfCursor action);
void call_c_init(int x, int y, int scale);
void call_set_colors(uint32_t textColor, uint32_t backgroundColor);
void call_get_registers();
void call_draw_square(uint32_t color, uint32_t x, uint32_t y, uint32_t scale);
void call_color_screen(uint32_t hexColor);
void call_draw_circle(uint32_t color, uint32_t x, uint32_t y, uint32_t length, uint32_t background);
void call_clear_screen();
void call_sleep(unsigned long long ms);
void call_get_ticks(unsigned long long * ticks);
void call_beep(uint32_t frequency);

void * call_malloc(uint64_t size);
void call_free(void * ptr);
int32_t call_create_process_foreground(char* name, Function function, char **args, uint32_t parentPid, int * fileDescriptors);
int64_t call_kill_process(uint32_t pid);
ProcessCopyList * call_get_process_copy();
uint32_t call_get_pid();
uint32_t call_get_parent_pid();
int64_t call_set_priority(uint32_t pid, uint64_t priority);
int64_t call_block(uint32_t pid);
int call_waitpid(uint32_t pid);
void call_free_process_copy(ProcessCopyList * processCopyList);
int32_t call_create_process_background(char* name, Function function, char **args, uint32_t parentPid,  int * fileDescriptors);
int call_get_pipe_id();
int16_t call_pipe_open(int id, char mode);
int16_t call_pipe_close(int id);
int16_t call_pipe_write(int id, char* msg, int len);
int16_t call_pipe_read(int id, char* msg, int len, uint32_t * readBytes);
//MemoryData * call_get_mem_info();
uint64_t call_sem_open(uint64_t value, uint64_t semId);
uint8_t call_sem_close(uint64_t semId);
uint64_t call_sem_wait(uint64_t semId);
uint64_t call_sem_post(uint64_t semId); 
void call_yield();
void call_sleep_seconds(unsigned long long seconds);

#endif
