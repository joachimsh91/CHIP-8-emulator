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
        case 0x1000: // 1NNN: Jumps to address NNN
            system->pc = system->opcode & 0x0FFF;
            break;

        case 0x2000: // 2NNN: Calls subroutine at NNN

            // Stores the return address for the next instruction on the stack where the stack pointer points
            system->stack[system->sp] = system->pc + 2;

            // Increases the stack pointer so it's ready for the next function call
            system->sp++;

            // Jump to the new address NNN
            system->pc = system->opcode & 0x0FFF;
            break;

        case 0x3000: { //Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            if (system->V[x] == nn){
                system->pc +=2;
            }
            system->pc +=2;
            break;
    }
        case 0x4000: { //Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            if (system->V[x] != nn){
                system->pc +=2;
            }
            system->pc +=2;
            break;
    }
        case 0x5000: { //Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t y = (system->opcode & 0x00F0) >> 4;
            if (system->V[x] == system->V[y]){
                system->pc +=2;
            }
            system->pc +=2;
            break;
        }

        case 0x6000: { //Sets VX to NN
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            system->V[x] = nn;
            system->pc +=2;
            break;

        }

        case 0x7000: { //Adds NN to VX (carry flag is not changed)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            system->V[x] += nn;
            system->pc +=2;
            break;
        }

        case 0xA000: // ANNN: Sets I to the address NNN
            system->I = system->opcode & 0x0FFF;
            system->pc += 2;
            break;

        case 0xB000: // BNNN: Jumps to the address NNN plus V0
            system->pc = (system->opcode & 0x0FFF) + system->V[0];
            break;
    
        default:
        printf("Unknown opcode: 0x%X\n", system->opcode);
    }  
}
