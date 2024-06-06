#ifndef SYSCALL_FUNCTIONS_H
#define SYSCALL_FUNCTIONS_H

#include <stddef.h>
#include <stdint.h>
#include <globals.h>



typedef enum {WRITE=0, DELETE, ENTER, RELATIVE_ENTER, TAB}actionOfCursor;

void call_read(uint8_t * buf, uint32_t count, uint32_t * readBytes);
void call_write(uint8_t * buf, uint32_t * length);
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
uint32_t call_create_process(char* name, char position, uint64_t priority, Function function, char **args, uint32_t parentPid);
void call_kill_process(uint32_t pid);
ProcessCopyListADT call_get_process_copy();
uint32_t call_get_pid();
uint32_t call_get_parent_pid();
void call_set_priority(uint32_t pid, uint64_t priority);
uint64_t call_set_state(uint32_t pid, uint64_t state);
uint64_t call_waitpid(uint32_t pid);

#endif
