.syntax unified
.cpu cortex-m4
.thumb

.global g_pfnVectors
.global Reset_Handler

.extern main
.extern _estack
.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss


/* Vector table */
.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object

g_pfnVectors:
    .word _estack
    .word Reset_Handler

.size g_pfnVectors, .-g_pfnVectors


/* Reset handler */
.section .text.Reset_Handler
.type Reset_Handler, %function
.thumb_func

Reset_Handler:

    /* Copy .data from FLASH to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata

CopyDataLoop:
    cmp r0, r1
    bcs CopyDataDone

    ldr r3, [r2]
    str r3, [r0]

    adds r0, r0, #4
    adds r2, r2, #4

    b CopyDataLoop

CopyDataDone:

    /* Zero .bss */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0

ZeroBssLoop:
    cmp r0, r1
    bcs ZeroBssDone

    str r2, [r0]
    adds r0, r0, #4

    b ZeroBssLoop

ZeroBssDone:

    bl main

Hang:
    b Hang

.size Reset_Handler, .-Reset_Handler
