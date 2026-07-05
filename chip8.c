#include "chip8.h"

void chip8_initialize(Chip8* system) {
    system->pc = 0x200; // Program counter always starts at 0x200
    system->opcode = 0;
    system->I = 0;
    system->sp = 0;

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
