.intel_syntax noprefix

.section .text
.global _set_instruction_ptr
_set_instruction_ptr:
    push ebp
    mov esp, ebp

    mov eax, [esp - 8]
    call eax
    
    mov ebp, esp    
    pop ebp
    ret
