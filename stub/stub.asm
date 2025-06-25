BITS 64
global _start

_start:

    ; push original_entry
    mov rax, 0x0000000000000000        
    push rax

    ; push text_size
    mov rax, 0x0000000000000000       
    push rax

    ; push text_start
    mov rax, 0x0000000000000000      
    push rax

    ; pop arguments
    pop rsi        ; text_start
    pop rcx        ; text_size

decode_loop:
    xor byte [rsi], 0x0     ;XOR with the provided KEY
    inc rsi
    loop decode_loop

    pop rax        ; original_entry
    jmp rax
