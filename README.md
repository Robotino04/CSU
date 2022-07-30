# CSU - Central Subtraction Unit

CSU is a CPU design built to work with only one instruction: a subtraction.
if the result of the subtraction is negative or zero, execution branches to a given address.

## Assembler
The custom assembler for the CSU is divided into five stages:
1. tokenising: converting the source code into tokens
2. token expanding: inlining pseudo instructions and implicit arguments
3. token positioning: establishing the position and size of each token
4. label resolution: replacing labels by their corresponding address
5. binary generation: converting the tokens into the final binary


## Resources
- [https://en.wikipedia.org/wiki/One-instruction_set_computer#Subtract_and_branch_if_less_than_or_equal_to_zero]()