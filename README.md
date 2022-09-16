# CSU - Central Subtraction Unit

CSU is a CPU design built to work with only one instruction: a subtraction.

## Architecture
The CSU is a 8-bit CPU with a 16-bit address space. The only instruction is a subtraction that
subtracts the data at the first address from the data at the second address and stores the result at the
second address. If the result is negative or zero, execution branches to the address speccified by the third
operand. Addresses are stored as little-endian. Jumping out of the program memory will halt the CPU.

## Assembler
The custom assembler for the CSU is divided into six stages:
1. tokenising: converting the source code into tokens
2. token expanding: inlining pseudo instructions and implicit arguments
3. token positioning: establishing the position and size of each token
4. label resolution: replacing labels by their corresponding address
5. evaluation: evaluating label offsets and constant values
6. binary generation: converting the tokens into the final binary

## Memory Layout

| $0x0000 - $0x7FFF | $0x8000  | $0x8001 |
| ----------------- | -------- | ------- |
| Program memory    | Char Out | Char In |

## IO
Reading from **Char In** will give the char code of the oldest character in a buffer. If the buffer is empty, NULL is read or it is waited until there is a character.

Writing to **Char In** will do nothing.

Writing to **Char Out** will write character to the output.

Reading from **Char Out** will yield zero.

In the emulator **Char In** and **Char Out** are links to stdin and stdout respectively.

## Resources
- [https://en.wikipedia.org/wiki/One-instruction_set_computer#Subtract_and_branch_if_less_than_or_equal_to_zero]()