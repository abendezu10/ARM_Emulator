.syntax unified
.cpu cortex-m4
.thumb

.global _start

_start:
  movs r0, #3 @ move immediate value, 5, into register 0
  movs r1, #5 @ move immediate value, 3, into register 1
  adds r2, r0, r1

loop:
  b loop
