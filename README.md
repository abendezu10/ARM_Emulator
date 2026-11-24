# ARM Thumb Emulator

A C++ emulator for the ARM Cortex-M Thumb instruction set.

The emulator loads a raw bare-metal ARM binary into virtual Flash memory, initializes the CPU from the vector table, and executes instructions using a fetch-decode-execute cycle.

The current version can execute a small C program compiled with `arm-none-eabi-gcc` for a Cortex-M4 target.

**Latest release: [v0.2.0](https://github.com/abendezu10/ARM_Emulator/releases/tag/v0.2.0)**

## Current functionality

The emulator currently supports:

- ARMv7-M / Cortex-M style register state
- R0-R12, SP, LR, and PC
- N, Z, C, V, and Q CPU flags
- Flash mapped at `0x08000000`
- SRAM mapped at `0x20000000`
- Little-endian 8, 16, and 32-bit memory access
- Initial SP and reset handler loading from the vector table
- Stack allocation and stack memory inspection
- 16-bit Thumb instruction decoding
- Initial 32-bit Thumb-2 instruction support
- Execution of simple bare-metal compiled C programs

## Supported instructions

Some of the currently implemented instruction paths include:

| Instruction | Example | Notes |
|---|---|---|
| MOVS | `movs r3, #3` | Immediate move, updates N/Z |
| ADDS | `adds r0, #4` | Addition with flags |
| SUBS | `subs r0, r1, r2` | Subtraction with flags |
| ADD | `add r3, r2` | Register addition |
| CMP | `cmp r0, r1` | Updates condition flags |
| LDR literal | `ldr r0, [pc, #imm]` | Loads constants from Flash |
| LDR | `ldr r2, [r7, #4]` | 32-bit SRAM load |
| STR | `str r3, [r7, #4]` | 32-bit SRAM store |
| PUSH | `push {r7}` | Stack register storage |
| ADD SP-relative | `add r7, sp, #0` | Used for stack frames |
| SUB SP | `sub sp, #12` | Stack allocation |
| B | `b loop` | Unconditional branch |
| B<cond> | `bcs label` | Conditional branch |
| BL | `bl main` | 32-bit Thumb-2 branch with link |

Instruction coverage is still incomplete and is being expanded gradually.

## Running compiled C

A test program can be compiled for Cortex-M4 using the GNU ARM toolchain.

Example source:

```c
volatile int result;

int main(void){
    int a = 3;
    int b = 5;

    result = a + b;

    while(1){}
}
```

The emulator currently executes the generated startup code and `main()`.

During execution:

```text
a = 3
b = 5

R2 = 3
R3 = 5

R3 = R3 + R2
R3 = 8

result = 8
```

The final value is written to SRAM at `0x20000000`.

## Memory map

The current memory layout models an STM32F401-style Cortex-M system.

| Region | Start | End |
|---|---|---|
| Flash | `0x08000000` | `0x0807FFFF` |
| SRAM | `0x20000000` | `0x20017FFF` |

The initial stack pointer is:

```text
0x20018000
```

which is one address past the top of SRAM, allowing the stack to grow downward.

## Fetch-decode-execute

The emulator follows a normal CPU execution cycle:

1. **Reset**
   - Read the initial SP from vector table offset `0x0`
   - Read the reset handler address from offset `0x4`

2. **Fetch**
   - Read a 16-bit Thumb instruction from Flash
   - Detect instructions that require a second Thumb halfword

3. **Decode**
   - Extract registers, immediates, opcode fields, and instruction type
   - Select an execute handler

4. **Execute**
   - Modify registers, flags, memory, or PC

5. **Repeat**

The decoder uses tables of C++ member-function pointers to dispatch instruction families.

## Stack debugging

The emulator includes a small SRAM stack viewer.

Example:

```text
┌─────────────────────────────┐
│    ADDRESS   │     VALUE    │
├─────────────────────────────┤
│ 0x20017ffc   │ 0x00000000   │
│ 0x20017ff8   │ 0x00000000   │
│ 0x20017ff4   │ 0x00000003   │
│ 0x20017ff0   │ 0x00000005   │
│ 0x20017fec   │ 0x00000000   │
└─────────────────────────────┘
```

This makes it possible to inspect stack variables while stepping through instructions.

## Project structure

| File | Purpose |
|---|---|
| `main.cpp` | Program entry point and emulator execution loop |
| `cpu.hpp/.cpp` | CPU registers, flags, fetch/decode/execute logic, and instruction handlers |
| `memory.hpp/.cpp` | Flash/SRAM implementation and memory accesses |
| `bin_file.hpp/.cpp` | Loads raw binary files |
| `test_cases/` | Bare-metal programs used to test the emulator |

## Build

```bash
make
```

or:

```bash
g++ -std=c++17 -Wall -Wextra main.cpp cpu.cpp memory.cpp bin_file.cpp -o emulator
```

## Run

```bash
./emulator program.bin
```

The current compiled-C test can be run with:

```bash
make test1
```

## Toolchain

The test binaries are built using the GNU ARM embedded toolchain.

Typical compiler options:

```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb
```

Useful tools during development include:

```text
arm-none-eabi-gcc
arm-none-eabi-as
arm-none-eabi-ld
arm-none-eabi-objcopy
arm-none-eabi-objdump
```

## Current limitations

This is still an educational emulator rather than a complete Cortex-M implementation.

Current limitations include:

- Only a subset of Thumb instructions is implemented
- Thumb-2 support is still limited
- No interrupt or exception handling yet
- No NVIC or SysTick emulation
- No peripheral/MMIO emulation yet
- No GPIO, UART, timers, or other STM32 peripherals
- Memory validation and alignment behavior are still incomplete
- Unsupported instruction handling still needs improvement
- More instruction-level tests are needed

## Roadmap

Planned work includes:

- Expand Thumb-16 instruction coverage
- Expand Thumb-2 support
- Add function return and additional stack instructions
- Add more compiled C test programs
- Add instruction-level automated tests
- Compare emulator results against QEMU
- Add memory-mapped peripheral support
- Add GPIO and basic STM32 peripherals
- Build a debugger-style interface for registers, memory, stack, and instructions

## Goal

The goal of this project is to learn how ARM Cortex-M processors execute real compiled programs by implementing the important pieces directly:

```text
binary
   ↓
Flash
   ↓
fetch
   ↓
decode
   ↓
execute
   ↓
registers / flags / SRAM
```

Rather than only interpreting hand-written instructions, the emulator is being developed toward running increasingly realistic bare-metal ARM programs.
