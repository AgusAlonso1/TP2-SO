GLOBAL _hlt
GLOBAL _cli
GLOBAL _sti
GLOBAL _create_stack_frame
GLOBAL picMasterMask
GLOBAL picSlaveMask

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq80Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscallsDispatcher
EXTERN schedule

EXTERN updateRegs

%macro pushState 0
	push rax
    push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
    pop rax
%endmacro



%macro irqHandlerMaster 1
    pushState


    mov rsi, rsp
    mov rdi, %1
    call irqDispatcher

    ; Se le avisa al PIC que se finalizo la atencion a la interrupcion.
    mov al, 20h
    out 20h, al

    popState
    iretq
%endmacro

%macro exceptionHandler 1
	pushState

    mov rsi, rsp
	mov rdi, %1  ; Pasaje de parametro
	call exceptionDispatcher

	popState
	iretq
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
    cli
    ret

_sti:
    sti
    ret


_create_stack_frame:
    mov r8, rsp 	; Preserva rsp
    mov r9, rbp		; Preserva rbp

    mov rsp, rdx 	; Setea el stack pointer en el stackEnd del proceso
    mov rbp, rdx    ; Setea el base pointer stackEnd del proceso

    ;Aling
    mov rax, rdx  ; rax = stackEnd
    add rax, 0x07    ; Ajusta para el alineamiento de 8 bytes
    and rax, 0xFFFFFFFFFFFFFFF8 ;-8 = 0xFFFFFFFFFFFFFFF8   Alinea a un múltiplo de 8 bytes

    push 0x0        ; Pushea el SS
    push rax        ; Pushea el RSP
    push 0x202      ; Pushea el RFLAGS
    push 0x8        ; pushea el CS
    push rdi        ; Pushea el RIP

    mov rdi, rsi 		; Guarda el puntero a la funcion (rip)
    mov rsi, rcx		; Guarda los args (args del code)

    pushState       ; Pushea los GPR
    mov rax, rsp    ; Devuelve el stack pointer luego de ser pusheado

    mov rsp, r8
    mov rbp, r9
    ret

picMasterMask:
    push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
    push rbp
    mov rbp, rsp
    mov ax, di
    out	0A1h,al
    pop rbp
    retn

; Timer Tick
_irq00Handler:
    ;irqHandlerMaster 0
	pushState

	mov rdi, 0          ; pasaje de parametro
	call irqDispatcher

	mov rdi, rsp        ; Recibe como parametro el stack pointer del proceso que se va a dejar de ejecutar
	call schedule       ; Llama al scheduler
	mov rsp, rax        ; Devuelve el stack pointer del proceso que se va a ejecutar

    ;send EOI
	mov al, 20h
	out 20h, al

	popState
	iretq

; Keyboard
_irq01Handler:
    irqHandlerMaster 1

; Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

; Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

; Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

; USB
_irq05Handler:
	irqHandlerMaster 5

_irq80Handler:      ; en rdi el mode, luego en el resto de los registros los parametros usuales de cada syscall
    push rbp
    push rsp
    push r12
    push r13
    push r15
    push r10

    call syscallsDispatcher

    pop r10
    pop r15
    pop r13
    pop r12
    pop rsp
    pop rbp
    iretq

; Exceptions

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

_exception6Handler:
    exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

SECTION .bss
	aux resq 1

