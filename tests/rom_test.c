#include <stdio.h>
#include <stdlib.h>
#include "../chip8.h" // Walks one directory up to find chip8.h

void print_buffer_data(Chip8* system, RomReader* rom) { // Helper function to verify that ROM data was loaded into buffer
    for (int i = 0; i < rom->actual_size; i++) {
        printf("Buffer[0x%X] = 0x%02X\n", i, rom->buffer[i]);
    }
}

void print_mem_data(Chip8* system, RomReader* rom) { // Helper function to verify that ROM data was loaded into memory
    for (int i = 0; i < rom->actual_size; i++) {
        printf("Memory[0x%X] = 0x%02X\n", 0x200 + i, system->memory[0x200 + i]);
    }
}

RomReader my_reader;
Chip8 my_chip8;

int main() {
    my_reader.filename = "tests/dummy_rom.ch8";
    
    printf("==================================================\n");
    printf("[STEP 1] Starting ROM reader for file: %s\n", my_reader.filename);
    printf("==================================================\n");
    rom_reader(&my_reader);
    
    printf("\n==================================================\n");
    printf("[STEP 2] Printing data from intermediate buffer\n");
    printf("==================================================\n");
    print_buffer_data(&my_chip8, &my_reader);
    
    printf("\n==================================================\n");
    printf("[STEP 3] Copying buffer data into emulator memory\n");
    printf("==================================================\n");
    rom_loader(&my_chip8, &my_reader);
    
    printf("\n==================================================\n");
    printf("[STEP 4] Printing final state of emulator memory\n");
    printf("==================================================\n");
    print_mem_data(&my_chip8, &my_reader);
    
    printf("\n==================================================\n");
    printf("[SUCCESS] All steps executed successfully!\n");
    printf("==================================================\n");
    
    return 0;
}
