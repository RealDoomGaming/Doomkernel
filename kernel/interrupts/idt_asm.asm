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