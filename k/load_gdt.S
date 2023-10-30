.section .text
.global load_gdt

load_gdt:

    /* Make new call frame */
    push %ebp           // save old call frame
    mov %esp, %ebp      // Initialize new call frame

    /* Load GDT */
    mov 8(%ebp), %eax
    lgdt (%eax)

    /* Reload code segment */
    mov 12(%ebp), %eax
    push %eax
    push $reload_cs
    retf

reload_cs:

    /* Reload data segments */
    movw 16(%ebp), %ax   
    movw %ax, %ds     
    movw %ax, %es     
    movw %ax, %fs     
    movw %ax, %gs     
    movw %ax, %ss

    /* Restore old call frame */
    mov %ebp, %esp
    pop %ebp
    ret