.section .text
.global load_idt

load_idt:
    pushl %ebp            /* Save ol call frame */
    mov %esp, %ebp        /* Initialize new call frame */

    // load idt
    movl 8(%ebp), %eax    /* load address of IDT from function argument */
    lidt (%eax)           /* load IDT */

    // restore old call frame
    movl %ebp, %esp
    popl %ebp
    ret 