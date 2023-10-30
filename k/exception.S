.section .text

.extern isr_exception_handler

.global exception_0
.global exception_1
.global exception_2
.global exception_3
.global exception_4
.global exception_5
.global exception_6
.global exception_7
.global exception_8
.global exception_9
.global exception_10
.global exception_11
.global exception_12
.global exception_13
.global exception_14
.global exception_15


exception_handler:
    pusha               // push all registers
    push %ds            // save ds 

    mov $0x10, %ax      // load kernel data segment        
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp

    call isr_exception_handler

	pop %ebx            // restore kernel data segment
	mov %ebx, %ds
	mov %ebx, %es
	mov %ebx, %fs
	mov %ebx, %gs

    popa                // restore all registers                    
    add $0x8, %esp      // restore stack for errno been pushed
                  
    iret


exception_0:
    push $0x0               /* push err_code(0) */
    push $0                 /* push exception number index in IDT */
    jmp exception_handler   /* handle exception */

exception_1:
    push $0x0               
    push $1               
    jmp exception_handler

exception_2:
    push $0x0               
    push $2               
    jmp exception_handler

exception_3:
    push $0x0               
    push $3               
    jmp exception_handler

exception_4:
    push $0x0               
    push $4               
    jmp exception_handler

exception_5:
    push $0x0               
    push $5               
    jmp exception_handler

exception_6:
    push $0x0               
    push $6               
    jmp exception_handler

exception_7:
    push $0x0               
    push $7               
    jmp exception_handler

exception_8:
    push $0x8               
    jmp exception_handler

exception_9:
    push $0x0               
    push $0x9               
    jmp exception_handler

exception_10:
    push $10              
    jmp exception_handler

exception_11:
    push $11              
    jmp exception_handler

exception_12:
    push $12              
    jmp exception_handler

exception_13:
    push $13              
    jmp exception_handler

exception_14:
    push $14              
    jmp exception_handler

exception_15:
    push $0x0              
    push $15              
    jmp exception_handler
