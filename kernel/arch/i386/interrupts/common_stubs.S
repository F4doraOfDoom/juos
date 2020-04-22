.intel_syntax noprefix

.macro isr_noerr num
    .global isr_\num
    .type isr_\num, @function
    isr_\num:
        cli 
        push 0
        push \num
        jmp isr_common_stub
.endm

.macro irq num, mapped
    .global irq_\num
    .type irq_\num, @function
    irq_\num:
        push 0
        push \mapped
        jmp irq_common_stub
.endm

.global isr_common_stub
.type isr_common_stub, @function
isr_common_stub:
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

.global irq_common_stub
.type irq_common_stub, @function
irq_common_stub:
    pusha 
    
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler

    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    sti
    iret

# cpu has 32 different basic interrupts
isr_noerr 0
isr_noerr 1
isr_noerr 2
isr_noerr 3
isr_noerr 4
isr_noerr 5
isr_noerr 6
isr_noerr 7
isr_noerr 8
isr_noerr 9
isr_noerr 10
isr_noerr 11
isr_noerr 12
isr_noerr 13
isr_noerr 14
isr_noerr 15
isr_noerr 16
isr_noerr 17
isr_noerr 18
isr_noerr 19
isr_noerr 20
isr_noerr 21
isr_noerr 22
isr_noerr 23
isr_noerr 24
isr_noerr 25
isr_noerr 26
isr_noerr 27
isr_noerr 28
isr_noerr 29
isr_noerr 30
isr_noerr 31

# create 16 different irq stubs
irq 0, 32
irq 1, 33
irq 2, 34
irq 3, 35
irq 4, 36
irq 5, 37
irq 6, 38
irq 7, 39
irq 8, 40
irq 9, 41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47
