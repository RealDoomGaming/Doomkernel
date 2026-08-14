[bits 64]

;; this file will be for handeling the interrupts in x86_64 asm
;; so basically we set up that when the cpu gives and interrupt who answers that one (I think?)

;; firstly we have to define some macros
;; this one is so when the cpu doesnt automatically push the error code onto the stack
;; we have to do it ourselfs
%macro ISR_NOERR 1
global isr%1
isr%1:
    push qword 0
    push qword %1
    jmp isr_common_stub
%endmacro

;; then the next macro
;; this one is for if the cpu already pushed the error code onto the stack and this only pushes the interrupt number onto the stack
%macro ISR_ERR 1
global isr%1
isr%1:
    push qword %1
    jmp isr_common_stub
%endmacro

;; then the last macro
;; this macro is for pushing hardware IRQs and not cpu exceptions like before
%macro IRQ 2
global irq%1
irq%1:
    push qword 0
    push qword %1
    jmp irq_common_stub
%endmacro

;; and now we have to define which exceptions from the cpu push an error and which dont
;; 0-7 use no error and only a hanfull (8, 10, 11, 12, 13, 14, 17, 30) use the ISR_ERR
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30
ISR_NOERR 31