#include "chip8.h"
#include "stdio.h"
#include "stdlib.h"

const uint8_t chip8_fontset[80] = {         // The standard CHIP-8 fontset (16 symbols, 5 bytes each)
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void rom_reader(RomReader* rom) { // Read ROM file and store ROM data to buffer 
    FILE *fil = fopen(rom->filename, "rb");
    if (fil == NULL) {
        printf("Error opening file: %s\n", rom->filename);
        return;
    }

    // Move the file pointer to the end to get file size
    fseek(fil, 0, SEEK_END);
    long file_size = ftell(fil);
    rom->actual_size = (uint16_t)file_size;
    rewind(fil); // Move back to the beginning 

    // Read the entire file into the buffer 
    size_t bytes_read = fread(rom->buffer, 1, file_size, fil);
    
    printf("Successfully read %zu bytes from %s into buffer.\n", bytes_read, rom->filename);

    // Clean up and close the file
    fclose(fil);
}

void rom_loader(Chip8* system, RomReader* rom) { // Load ROM data into memory
    for (int i = 0; i < rom->actual_size; i++) {
        system->memory[0x200 + i] = rom->buffer[i];
    }
}


void chip8_initialize(Chip8* system) {
    system->pc = 0x200;         // Program counter always starts at 0x200
    system->opcode = 0;         // Reset current opcode
    system->I = 0;              // Reset index register
    system->sp = 0;             // Reset stack pointer
    system->delay_timer = 0;    // Reset delay timer
    system->sound_timer = 0;    // Reset sound timer

    // Clear screen
    for (int i = 0; i < 2048; i++) {
        system->gfx[i] = 0;
    }

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

    // Load fontset into memory from address 0x50 and onwards
    for (int i = 0; i < 80; i++) {
        system->memory[0x50 + i] = chip8_fontset[i];
    }
}

void chip8_emulateCycle(Chip8* system) {

    // Fetch opcode
    system->opcode = system->memory[system->pc] << 8 | system->memory[system->pc + 1];

    // Decode opcode
    switch(system->opcode & 0xF000)
    {   
        case 0x0000:
            switch(system->opcode & 0x00FF)
            {
                case 0x00E0: // Clears the screen
                    for (int i = 0; i < 2048; i++) {
                        system->gfx[i] = 0;
                    }
                    system->pc += 2;
                    break;

                case 0x00EE: // Returns from a subroutine
                    system->sp--; // Move stack pointer down to the saved address
                    system->pc = system->stack[system->sp]; // // Put the return address back into PC
                    break;
            }
            break;

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

        case 0x3000: { // Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            if (system->V[x] == nn){
                system->pc += 2;
            }
            system->pc += 2;
            break;
    }
        case 0x4000: { // Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            if (system->V[x] != nn){
                system->pc += 2;
            }
            system->pc += 2;
            break;
    }
        case 0x5000: { // Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t y = (system->opcode & 0x00F0) >> 4;
            if (system->V[x] == system->V[y]){
                system->pc += 2;
            }
            system->pc += 2;
            break;
        }

        case 0x6000: { // Sets VX to NN
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            system->V[x] = nn;
            system->pc += 2;
            break;

        }

        case 0x7000: { // Adds NN to VX (carry flag is not changed)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            system->V[x] += nn;
            system->pc += 2;
            break;
        }

        case 0x8000: {
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t y = (system->opcode & 0x00F0) >> 4;

            switch(system->opcode & 0x000F)
            {
                case 0x0000: // Sets VX to the value of VY
                    system->V[x] = system->V[y];
                    system->pc += 2;
                    break;
                
                case 0x0001: // Sets VX to VX or VY (bitwise OR operation)
                    system->V[x] = system->V[x] | system->V[y];
                    system->pc += 2;
                    break;

                case 0x0002: // Sets VX to VX and VY (bitwise AND operation)
                    system->V[x] = system->V[x] & system->V[y];
                    system->pc += 2;
                    break;
                
                case 0x0003: // Sets VX to VX xor VY 
                    system->V[x] = system->V[x] ^ system->V[y];
                    system->pc += 2;
                    break;

                case 0x0004: { // Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
                    uint16_t sum = system->V[x] + system->V[y];
                    if (sum > 255) {
                        system->V[0xF] = 1;
                    }
                    else {
                        system->V[0xF] = 0;
                    }
                    system->V[x] = sum & 0xFF;
                    system->pc += 2;
                    break;
                }

                case 0x0005: { // VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not
                    if (system->V[x] >= system->V[y]) {
                        system->V[0xF] = 1;
                    }
                    else {
                        system->V[0xF] = 0;
                    }
                    system->V[x] = (system->V[x] - system->V[y]) & 0xFF;
                    system->pc += 2;
                    break;
                }

                case 0x0006: { // Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
                    uint8_t lsb = system->V[x] & 0x01;
                    system->V[0xF] = lsb;
                    system->V[x] = system->V[x] >> 1;
                    system->pc += 2;
                    break;
                }

                case 0x0007: { // Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not
                    if (system->V[y] >= system->V[x]) {
                        system->V[0xF] = 1;
                    }
                    else {
                        system->V[0xF] = 0;
                    }
                    system->V[x] = system->V[y] - system->V[x];
                    system->pc += 2;
                    break;
                }

                case 0x000E: { // Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset
                    if ((system->V[x] & 0x80) != 0) {
                        system->V[0xF] = 1; 
                    } 
                    else {
                        system->V[0xF] = 0;
                    }
                    system->V[x] = (system->V[x] << 1) & 0xFF;
                    system->pc += 2;
                    break;
                }
            }
            break;
        }

        case 0x9000: { // Skips the next instruction if VX does not equal VY (usually the next instruction is a jump to skip a code block)
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t y = (system->opcode & 0x00F0) >> 4;
            if (system->V[x] != system->V[y]){
                system->pc +=2;
            }
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

        case 0xC000: { // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            uint8_t nn = system->opcode & 0x00FF;
            uint8_t rand_num = (rand() % 256);
            system->V[x] = rand_num & nn;
            system->pc +=2;
            break;
        }

        case 0xE000: {
            uint8_t x = (system->opcode & 0x0F00) >> 8;

            switch(system->opcode & 0x00FF)
            {
                case 0x009E: // Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block)
                    if (system->key[system->V[x]] == 1){
                        system->pc +=2;
                    }
                    system->pc +=2;
                    break;

                case 0x00A1: // Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block)
                    if (system->key[system->V[x]] == 0){
                        system->pc +=2;
                    }
                    system->pc +=2;
                    break;
            }
            break;
        }

        case 0xF000: {
            uint8_t x = (system->opcode & 0x0F00) >> 8;
            switch(system->opcode & 0x00FF)
            {
                case 0x0007:
                    system->V[x] = system->delay_timer;
                    system->pc += 2;
                    break;
                
                case 0x000A: { // FX0A: Wait for a key press, store the value of the key in VX
                    int key_pressed = 0;

                    for (int i = 0; i < 16; i++) {
                        if (system->key[i] == 1) {
                            system->V[x] = i;  
                            key_pressed = 1;   
                            break;             
                        }
                    }

                    if (key_pressed) {
                        system->pc += 2;
                    }
                    break;
                }

                case 0x0015: // Sets the delay timer to VX
                    system->delay_timer = system->V[x];
                    system->pc += 2;
                    break;

                case 0x0018: // Sets the sound timer to VX
                    system->sound_timer = system->V[x];
                    system->pc += 2;
                    break;

                case 0x001E: // Adds VX to I. VF is not affected
                    system->I = system->I + system->V[x];
                    system->pc += 2;
                    break;

                case 0x0029: // Sets I to the location of the sprite for the character in VX
                    system->I = 0x50 + (system->V[x] * 5);
                    system->pc += 2;
                    break;
                
                case 0x0033: { // FX33: Stores the BCD representation of VX in memory at I, I+1, and I+2
                    uint8_t value = system->V[x];

                    system->memory[system->I] = value / 100;
                    system->memory[system->I + 1] = (value / 10) % 10;
                    system->memory[system->I + 2] = value % 10;
                    system->pc += 2;
                    break;
                }

                case 0x0055: { // Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
                    for (int i = 0; i <= x; i++){
                        system->memory[system->I +i] = system->V[i];
                    }
                    system->pc += 2;
                    break;
                }

                case 0x0065: { // Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
                    for (int i = 0; i <= x; i++){
                        system->V[i] = system->memory[system->I +i];
                    }
                    system->pc += 2;
                    break;
                }
            }
            break;
        }
    
        default:
        printf("Unknown opcode: 0x%X\n", system->opcode);
    }  
}
