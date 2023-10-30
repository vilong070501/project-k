.section .text
.global load_gdt

load_gdt:
    movw $0x10, %ax   
    movw %ax, %ds     
    movw %ax, %es     
    movw %ax, %fs     
    movw %ax, %gs     
    movw %ax, %ss     
    ljmp $0x08, $next 

next:      	      
    ret