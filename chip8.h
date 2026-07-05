#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

/*
Memory map:
0x000-0x1FF - CHIP-8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM

The graphics system: The CHIP-8 has one instruction that draws sprite to the screen. 
Drawing is done in XOR mode and if a pixel is turned off as a result of drawing, 
the VF register is set. This is used for collision detection.

It is important to know that the CHIP-8 instruction set has opcodes that allow the program to jump to a certain address or call a subroutine. 
While the specification doesn’t mention a stack, you will need to implement one as part of the interpreter yourself. 
The stack is used to remember the current location before a jump is performed. 
So anytime you perform a jump or call a subroutine, store the program counter in the stack before proceeding.

CHIP-8 has two timer registers that counts at 60Hz.
When set above zero they will count down to zero.
*/

// Struct that holds the states of the emulator
typedef struct {
    uint16_t opcode; // Data type to store two bytes long opcodes
    uint8_t memory[4096]; // 4K memory for CHIP-8
    uint16_t V[16]; // CHIP-8 has 15 8-bit general purpose registers named V0, V1 up to VE. The 16th register is used for the 'carry flag'.
    uint16_t I;   // Index register 'I' and program counter 'pc' which can have a value from 0x000 to 0xFFF
    uint16_t pc;  // Program counter
    uint8_t gfx[64 * 32]; // The graphics of the CHIP-8 are black and white and the screen has a total of 2048 pixels (64 x 32). 
    uint8_t delay_timer; // First timer register
    uint8_t sound_timer; // Second timer register
    uint16_t stack[16]; // The system has 16 levels of stack.
    uint16_t sp; // In order to remember which level of the stack is used, you need to implement a stack pointer (sp).
    uint8_t key[16]; //The CHIP-8 has a HEX based keypad (0x0-0xF), you can use an array to store the current state of the key. 
} Chip8;


void chip8_initialize(Chip8* system);

void chip8_emulateCycle(Chip8* system);

#endif
