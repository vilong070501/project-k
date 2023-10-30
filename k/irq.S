.section .text

.extern isr_irq_handler

.global irq_0
.global irq_1
.global irq_2
.global irq_3
.global irq_4
.global irq_5
.global irq_6
.global irq_7
.global irq_8
.global irq_9
.global irq_10
.global irq_11
.global irq_12
.global irq_13
.global irq_14
.global irq_15


irq_handler:
    pusha                   // push all registers
    push %ds                // save ds

    mov $0x10, %ax          // load kernel data segment
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp

    call isr_irq_handler

	pop %ebx                // restore kernel data segment
	mov %ebx, %ds
	mov %ebx, %es
	mov %ebx, %fs
	mov %ebx, %gs

    popa                    // restore all registers
    add $0x8, %esp          // restore stack for errno been pushed
               
    iret


irq_0:
    push $0x0               
    push $32               
    jmp irq_handler

irq_1:
    push $0x0               
    push $33               
    jmp irq_handler

irq_2:
    push $0x0               
    push $34               
    jmp irq_handler

irq_3:
    push $0x0               
    push $35               
    jmp irq_handler

irq_4:
    push $0x0               
    push $36               
    jmp irq_handler

irq_5:
    push $0x0               
    push $37               
    jmp irq_handler

irq_6:
    push $0x0               
    push $38               
    jmp irq_handler

irq_7:
    push $0x0               
    push $39               
    jmp irq_handler

irq_8:
    push $0x0
    push $40               
    jmp irq_handler

irq_9:
    push $0x0               
    push $41               
    jmp irq_handler

irq_10:
    push $0x0
    push $42              
    jmp irq_handler

irq_11:
    push $0x0 
    push $43              
    jmp irq_handler

irq_12:
    push $0x0 
    push $44              
    jmp irq_handler

irq_13:
    push $0x0
    push $45              
    jmp irq_handler

irq_14:
    push $0x0 
    push $46              
    jmp irq_handler

irq_15:
    push $0x0              
    push $47              
    jmp irq_handler
