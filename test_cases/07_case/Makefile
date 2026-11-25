test.bin: test.elf
	arm-none-eabi-objcopy -O binary test.elf test.bin 

test.elf: test.o startup.o linker.ld
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -nostdlib -T linker.ld startup.o test.o -o test.elf

test.o: test.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -ffreestanding -c test.c -o test.o

startup.o: startup.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c startup.s -o startup.o

clean:
	rm -f test.elf test.o startup.o test.bin
