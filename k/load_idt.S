.section .text
.global load_idt

load_idt:

    /* Make new call frame */
    push %ebp             // Save ol call frame
    mov %esp, %ebp        // Initialize new call frame

    /* Load IDT */
    mov 8(%ebp), %eax     // Load address of IDT from function argument
    lidt (%eax)           // Load IDT

    /* Restore old call frame */
    mov %ebp, %esp
    pop %ebp
    ret 