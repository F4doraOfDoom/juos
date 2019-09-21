.intel_syntax noprefix

.global gdt_dump
.type gdt_dump, @function
gdt_dump:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.end
.end:
    ret

.global idt_dump
.type idt_dump, @function
idt_dump:
    mov eax, [esp + 4]
    lidt [eax]
    ret

.global isr_basic_caller
.type isr_basic_caller, @function
isr_basic_caller:
    pusha 

    mov eax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    sti
    iret
