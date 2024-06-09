GLOBAL call_read
GLOBAL call_write
GLOBAL call_draw_char
GLOBAL call_delete_char
GLOBAL call_get_time
GLOBAL call_set_theme
GLOBAL call_c_get_x
GLOBAL call_c_get_y
GLOBAL call_c_get_scale
GLOBAL call_c_set_scale
GLOBAL call_c_move
GLOBAL call_c_init
GLOBAL call_set_colors
GLOBAL call_get_registers
GLOBAL call_draw_square
GLOBAL call_color_screen
GLOBAL call_draw_circle
GLOBAL call_clear_screen
GLOBAL call_set_exception_handler
GLOBAL call_sleep
GLOBAL call_get_ticks
GLOBAL call_beep
GLOBAL call_malloc
GLOBAL call_free
GLOBAL call_create_process_foreground
GLOBAL call_kill_process
GLOBAL call_get_process_copy
GLOBAL call_get_pid
GLOBAL call_get_parent_pid
GLOBAL call_set_priority
GLOBAL call_block
GLOBAL call_waitpid
GLOBAL call_free_process_copy
GLOBAL call_create_process_background
GLOBAL call_get_pipe_id
GLOBAL call_pipe_open
GLOBAL call_pipe_close
GLOBAL call_pipe_write
GLOBAL call_pipe_read
GLOBAL call_get_mem_info
GLOBAL call_sem_open
GLOBAL call_sem_close
GLOBAL call_sem_wait
GLOBAL call_sem_post
GLOBAL call_yield


section .text

%macro pushAll 0
	push rbx
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popAll 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
%endmacro

call_read:     ; call_read(uint8_t * buf, uint32_t count, uint32_t * readBytes);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx

    mov rcx, rdx        ; rcx -> readBytes
    mov rdx, rsi        ; rdx -> count
    mov rsi, rdi        ; rsi -> buffer
    mov rdi, 0
    int 80h

    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_write:        ;call_write(uint8_t * buf, uint32_t * length);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx

    mov rdx, rsi        ; rdx -> length
    mov rsi, rdi        ; rsi -> buf
    mov rdi, 1
    int 80h

    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_draw_char:         ;call_draw_char(uint8_t character);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> character
    mov rdi, 2
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_delete_char:   ;void call_delete_char();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 3
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_time:      ;void call_get_time(uint8_t ** time);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> time
    mov rdi, 4
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_set_theme:     ;call_set_theme(uint32_t * theme);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> theme
    mov rdi, 5
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_set_exception_handler:     ;call_set_exception_handler(uint64_t number, void * exception);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx

    mov rdx, rsi        ; rdx -> exception
    mov rsi, rdi        ; rsi -> number
    mov rdi, 6
    int 80h

    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_c_get_x:   ;void call_c_get_x(int * x);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> x
    mov rdi, 7
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

    call_c_get_y:   ;void call_c_get_y(int * y);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> y
    mov rdi, 8
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

    call_c_get_scale:   ;call_c_get_scale(int * scale);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> scale
    mov rdi, 9
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_c_set_scale:   ;call_c_set_scale(int scale);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> scale
    mov rdi, 10
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_c_move:    ;call_c_move(actionOfCursor action);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> action
    mov rdi, 11
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_c_init:    ;call_c_init(int x, int y, int scale);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx

    mov rcx, rdx        ; rcx -> scale
    mov rdx, rsi        ; rdx -> y
    mov rsi, rdi        ; rsi -> x
    mov rdi, 12
    int 80h

    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_set_colors:    ;call_set_colors(uint32_t textColor, uint32_t backgroundColor);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx

    mov rdx, rsi        ; rdx -> backgroundColor
    mov rsi, rdi        ; rsi -> textColor
    mov rdi, 13
    int 80h

    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_registers:     ;call_get_registers();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 14
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_draw_square:   ;call_draw_square(uint32_t color, uint32_t x, uint32_t y, uint32_t scale);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx
    push r8


    mov r8, rcx			; r8 -> scale
    mov rcx, rdx        ; rcx -> y
    mov rdx, rsi        ; rdx -> x
    mov rsi, rdi        ; rsi -> color
    mov rdi, 15
    int 80h

    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_color_screen:      ;call_color_screen(uint32_t hexColor);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> hexColor
    mov rdi, 16
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_draw_circle:   ;call_draw_circle(uint32_t color, uint32_t x, uint32_t y, uint32_t length, uint32_t background);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9

    mov r9, r8			; r9 -> background
    mov r8, rcx			; r8 -> length
    mov rcx, rdx        ; rcx -> y
    mov rdx, rsi        ; rdx -> x
    mov rsi, rdi        ; rsi -> color
    mov rdi, 17
    int 80h

    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_clear_screen:      ;call_clear_screen();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 18
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_sleep:     ;call_sleep(unsigned long long ms);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> ms
    mov rdi, 19
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_ticks:     ;call_get_ticks(unsigned long long * ticks);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> ticks
    mov rdi, 20
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_beep:  ;call_beep(uint32_t frequency);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> frequency
    mov rdi, 21
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_malloc:    ;call_malloc(uint64_t size);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> size
    mov rdi, 22
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret


call_free:  ;call_free(void * ptr);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> ptr
    mov rdi, 23
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_create_process_foreground:    ;call_create_process(char* name, Function function, char **args, uint32_t parentPid, const uint64_t * filedescriptors);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9

    mov r9, r8			; r9 -> filedescriptors
    mov r8, rcx			; r8 -> parentPid
    mov rcx, rdx        ; rcx -> args
    mov rdx, rsi        ; rdx -> function
    mov rsi, rdi        ; rsi -> name
    mov rdi, 24
    int 80h

    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_kill_process:      ;call_kill_process(uint32_t pid);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> pid
    mov rdi, 25
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_process_copy:  ;call_get_process_copy();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 26
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_pid:   ;call_get_pid();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 27
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_parent_pid:    ;call_get_parent_pid();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 28
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_set_priority:  ;call_set_priority(uint32_t pid, uint64_t priority);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx

    mov rdx, rsi        ; rdx -> priority
    mov rsi, rdi        ; rsi -> pid
    mov rdi, 29
    int 80h

    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_block:     ;call_set_state(uint32_t pid, uint64_t state);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> pid
    mov rdi, 30
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_waitpid:   ;call_waitpid(uint32_t pid);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> pid
    mov rdi, 31
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_free_process_copy:   ;call_free_process_copy(ProcessCopyList * pcl);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> pcl
    mov rdi, 32
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_create_process_background:    ;call_create_process_background(char* name, Function function, char **args, uint32_t parentPid, const uint64_t * fileDescriptors);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9

    mov r9, r8			; r9 -> filedescriptors
    mov r8, rcx			; r8 -> parentPid
    mov rcx, rdx        ; rcx -> args
    mov rdx, rsi        ; rdx -> function
    mov rsi, rdi        ; rsi -> name
    mov rdi, 33
    int 80h

    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_pipe_id:    ;call_get_pipe_id();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 34
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_pipe_open:    ;call_pipe_open(int id, char mode);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx

    mov rdx, rsi        ; rdx -> mode
    mov rsi, rdi        ; rsi -> id
    mov rdi, 35
    int 80h

    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_pipe_close:    ;call_pipe_close(int id);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi        ; rsi -> id
    mov rdi, 36
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_pipe_write:    ;call_pipe_write(int id, char* msg, int len);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx

    mov rcx, rdx        ; rcx -> len
    mov rdx, rsi        ; rdx -> msg
    mov rsi, rdi        ; rsi -> id
    mov rdi, 37
    int 80h

    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_pipe_read:    ;call_pipe_read(int id, char* msg, int len, uint32_t * readBytes);
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx
    push r8

    mov r8, rcx			; r8 -> readbytes
    mov rcx, rdx        ; rcx -> len
    mov rdx, rsi        ; rdx -> msg
    mov rsi, rdi        ; rsi -> id
    mov rdi, 38
    int 80h

    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_get_mem_info:        ;call_get_mem_info();
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 39
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_sem_open:
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx

    mov rdx, rsi
    mov rsi, rdi
    mov rdi, 40
    int 80h

    pop rdx
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_sem_close:
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi
    mov rdi, 41
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_sem_wait:
     push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi
    mov rdi, 42
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_sem_post:
    push rbp
    mov rbp, rsp

    push rdi
    push rsi

    mov rsi, rdi
    mov rdi, 43
    int 80h

    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

call_yield:
    push rbp
    mov rbp, rsp

    push rdi

    mov rdi, 44
    int 80h

    pop rdi

    mov rsp, rbp
    pop rbp
    ret