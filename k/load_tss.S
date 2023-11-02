.section .text

.global load_tss

load_tss:
    movw 0x2B, %ax
    ltr %ax
    ret