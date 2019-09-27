.intel_syntax noprefix

.global idt_dump
.type idt_dump, @function
idt_dump:
    mov eax, [esp + 4]
    lidt [eax]
    ret
