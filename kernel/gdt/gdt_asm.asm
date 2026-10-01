;; this is the function which we can call in the c code
global load_gdt
;; and this is also a function which we can call from the c code
global load_tss

;; this label is supposed to load the gdt
load_gdt:
    ;; so firstly we load the gdt pointer so the cpu knows where the gdt table is
    lgdt [rdi]

    ;; then we have to reload all data segment registers
    ;; with the 0x10 we gave the load gdt function
    mov ax, dx              
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;; then after doing that we also have to reload cs with a far jump
    push rsi    ;; firstly we push the code selector
    lea rax, [.reload_cs]   ;; then we actually perform the long jump
    push rax    ;; then we push the return address
    retfq       ;; and do a far return

.reload_cs:
    ret;

;; and this is the label for the tss
load_tss:
    mov ax, di      ;; here we firstly load di into ax because in di we have the tss segment (0x28)
    ltr ax          ;; then we load the tss register
    ret             ;; and return