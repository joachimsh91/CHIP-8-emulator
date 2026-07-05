#include "chip8.h"

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
