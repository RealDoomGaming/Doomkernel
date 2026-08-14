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
;; 0-31 are the reserved exceptions from the cpu
;; only a handfull (8, 10, 11, 12, 13, 14, 17, 30) use the ISR_ERR
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

;; the remaining 16 interrupts are hardware interrupt so 32-47
IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

;; now come the stubs we use in the macros
isr_common_stub:
    ;; firstly we push all gp registers to the stack preserving the cpu state
    call save_cpu
    ;; then we put our first argument of the c function we will call later into rdi
    ;; and when we do this we set rdi to the current stack pointer so the c function will get that later
    mov rdi, rsp
    call isr_handler
    ;; and then after all that we restore the cpu state from before
    call restore_cpu

    ;; but lastly we clean up the code by
    ;; cleaning up the pushes from earlier 
    add rsp, 16
    ;; and call the special return from interrupt instruction which 
    ;; pop rip, cs, rflags, rsp and ss off the stack
    iretq

irq_common_stub:
    ;; this is basically the same as the irs label
    call save_cpu

    mov rdi, rsp
    ;; we only call the irq handler here
    call irq_handler

    call restore_cpu

    add rsp, 16
    iretq

;; and lastly we have a wrapper so the c code can call this like a normal function
global idt_flush
idt_flush:
    ;; rdi hold a pointer to an IDT descriptor struct and the lidt instruction loads that into the cpus
    ;; IDTR register telling the cpu that we now have a idt and the cpu can use it
    lidt [rdi]
    ret

save_cpu:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ret

restore_cpu:
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ret