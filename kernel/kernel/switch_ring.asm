[bits 64]
global enter_user_mode

;; this label is for going from ring 0 to ring 3 with an interrupt
;; it takes a few parameters:
;; 1. the user entry point
;; 2. the user stack
;; 3. the user cs (code segement)
;; 4. the user ds (data segment)
enter_user_mode:
    ;; then firstly we need to load cx into all data segment registers
    ;; because they specify what segement descriptor in the gdt governs data operations
    mov ds, cx
    mov es, cx 
    mov fs, cx
    mov gs, cx

    ;; then secondly we need to construct the iretq stack frame so we can call it later
    push rcx    ;; so we firstly push the user data selector onto the stack
    push rsi    ;; and also push the user stack top onto the stack

    ;; then we read the current RFlags onto the stack
    pushfq
    pop rax     ;; pop them off the stack
    or rax, 0x200   ;; modify the 9th bit to set it to 1 so we enable interrupts in Ring 3
    push rax    ;; and then we push it back onto the stack

    ;; lastly we push the user code selector and the user entry point onto the stack
    push rdx
    push rdi

    ;; and then we can perform an interrupt so we jump to ring 3
    iretq