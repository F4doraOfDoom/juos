.section .text
.global get_ip
.type get_ip, @function
get_ip:
    pop %eax;
    jmp %eax;

    