/*.intel_syntax noprefix

// ; signiture 
// ;    extern void context_switch(uint32_t esp, uint32_t ebp, paging::PageDirectory* dir, uint32_t pid, uint32_t times_ran, void* on_end, uint32_t next_eip);


.section .text
.global context_switch
.type context_switch, @function
context_switch:
    push ebp
    mov ebp, esp
    
    //; load esp, ebp
    mov eax, [ebp - 28]
    mov ebx, [ebp - 24]
    mov eax, esp
    mov ebx, ebp

    mov esi, [ebp - 20] // ; directory
    mov eax, [ebp - 16] // ; pid 
    mov ebx, [ebp - 12] // ; times_ran
    mov ecx, [ebp - 8] // ; on_end
    mov edx, [ebp - 4] // ; next_eip

    // ; change directory
    push esi
    push eax
    call SetDirectory

    cmp ebx, 1
    jne .not_first_time
        push eax
        push 0
        push ecx

.not_first_time:
    sti
    jmp edx


