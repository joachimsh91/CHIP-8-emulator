#include "chip8.h"
#include "stdio.h"

void chip8_initialize(Chip8* system) {
    system->pc = 0x200;         // Program counter always starts at 0x200
    system->opcode = 0;         // Reset current opcode
    system->I = 0;              // Reset index register
    system->sp = 0;             // Reset stack pointer
    system->delay_timer = 0;    // Reset delay timer
    system->sound_timer = 0;    // Reset sound timer

    // Clear stack
    for (int i = 0; i < 16; i++) {
        system->stack[i] = 0;
    }

    // Clear registers V0-VF
    for (int i = 0; i < 16; i++) {
        system->V[i] = 0;
    }

    // Clear memory
    for (int i = 0; i < 4096; i++) {
        system->memory[i] = 0;
    }
}

void chip8_emulateCycle(Chip8* system) {

    // Fetch opcode
    system->opcode = system->memory[system->pc] << 8 | system->memory[system->pc + 1];

    // Decode opcode
    switch(system->opcode & 0xF000)
    {    
        // Some opcode
    
        case 0xA000: // ANNN: Sets I to the address NNN

        // Execute opcode
        system->I = system->opcode & 0x0FFF;
        system->pc += 2;
        break;
    
        // More opcodes //
    
        default:
        printf("Unknown opcode: 0x%X\n", system->opcode);
    }  
}
