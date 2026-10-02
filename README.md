# ARM Thumb Emulator

A small C++ emulator for ARM Thumb (16-bit) instructions. It loads a raw `.bin` file into virtual memory, then fetches, decodes, and executes instructions one at a time, like a tiny Cortex-M CPU.

This is a learning project. It only supports a few instructions so far.

## Supported instructions

| Instruction | Example | Notes |
|---|---|---|
| MOVS Rd, #imm | `movs r0, #3` (`0x2003`) | 8-bit immediate |
| ADDS Rd, Rn, Rm | `adds r0, r1, r2` | register add |
| SUBS Rd, Rn, Rm | `subs r0, r1, r2` | register subtract |
| B label | `b loop` | unconditional branch |

Anything else is treated as a NOP and prints a message.

## How it works

The program runs in a simple loop:

1. **Load**: `BinFile` opens the binary and reads its size.
2. **Memory**: `Memory` copies every byte of the file into a `std::vector<uint8_t>`. Reads and writes are little-endian.
3. **Fetch**: `Cpu::fetch16` reads 16 bits at the PC into the instruction register, then moves the PC forward by 2.
4. **Decode**: the top 6 bits of the instruction (`bits 15:10`) are used as an index into a 64-entry table of function pointers. Each entry points to a decode function that fills in a `DecodedInstruction` (opcode, registers, immediate, execute handler).
5. **Execute**: the CPU calls the execute handler that decode picked.
6. The loop in `main` repeats until the PC reaches the end of the file.

## Files

| File | What it does |
|---|---|
| `main.cpp` | Entry point. Sets up the file, memory, and CPU, then runs the cycle loop. |
| `bin_file.hpp/.cpp` | Opens the binary file and returns its size and bytes. |
| `memory.hpp/.cpp` | Virtual memory with 8, 16, and 32-bit reads and writes. |
| `cpu.hpp/.cpp` | Registers (R0-R12, SP, LR, PC), the fetch/decode/execute logic, and the instruction handlers. |

## Build

Needs a compiler with C++17 (for `std::filesystem`).

```bash
g++ -std=c++17 -Wall -Wextra *.cpp -o emulator
```

## Run

```bash
./emulator program.bin
```

Example: a file containing the bytes `03 20` is `movs r0, #3`, so after one cycle R0 will be 3.

## Known limitations and TODO

- No 32-bit Thumb-2 instructions yet. The decoder spots them but does nothing.
- No condition flags (N, Z, C, V), so no conditional branches.
- The initial SP and reset vector are not loaded from the start of the binary yet. The PC starts at 0.
- `write_16bit` and `write_32bit` write every byte to the same address. They need to use `addr`, `addr + 1`, and so on.
- No bounds checking on memory reads and writes.
- The add/sub decoder masks register fields with `0xff` instead of `0x7`.
- Debug `cout` prints are still in the fetch and decode steps.
