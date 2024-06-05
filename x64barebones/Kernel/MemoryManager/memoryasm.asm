GLOBAL allocMemory
GLOBAL free
section .text

;=========================================================
;   Argumentos: 0
;   Devuelve: Devuelve un puntero a ese espacio de memoria
;=========================================================


allocMemory:
    push rbp
	mov rbp, rsp

    push rbx
    push rcx
    push rdx
    mov rbx, chunk_size_128
    mov rcx, cant_chunks_128
    mov rdx,chunk2048


.loop: cmp rcx, 0
    je .error
    mov al, [rdx]
    cmp al, 0
    je .fin
    dec rcx
    add rdx, rbx
    jmp .loop



.fin:
    mov rax, rdx
    mov BYTE[rdx], 1
    inc rax             ; Aumento el puntero para no pisar la flag

.ret:pop rdx
    pop rcx
    pop rbx
    mov rsp, rbp
    pop rbp
    ret

.error:
    mov rax, -1
    jmp .ret


;=========================================================
;   Argumentos: Puntero a la zona a liberar
;   Devuelve: Devuelve un puntero a ese espacio de memoria
;=========================================================


freeMemory:
    push rbp
	mov rbp, rsp

    dec rdi
    mov BYTE[rdi], 0
    inc rdi
    mov rsp, rbp
    pop rbp
    ret


section .data

chunk_size_128 dq 128
cant_chunks_128 dq 160

section .bss

chunk2048 resb 20480
; Puede guardar 10 chunks de 2048 bytes