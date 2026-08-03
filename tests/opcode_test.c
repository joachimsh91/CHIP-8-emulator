#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../chip8.h" // Walks one directory up to find chip8.h

// Helper function to inject a 16-bit opcode into memory at a specific address
void inject_opcode(Chip8* system, uint16_t addr, uint16_t opcode) {
    system->memory[addr] = (opcode & 0xFF00) >> 8;
    system->memory[addr + 1] = opcode & 0x00FF;
}

int main() {
    Chip8 my_chip8;
    chip8_initialize(&my_chip8);
    
    // Seed the random number generator with the current time
    srand(time(NULL));
    
    printf("=== STARTING COMPREHENSIVE OPCODE TEST (33/34 OPCODES) ===\n\n");

    // ----------------------------------------------------
    // TEST 1: Core assignment and arithmetic (6XNN, 7XNN)
    // ----------------------------------------------------
    my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x6110); // V1 = 0x10 (16)
    inject_opcode(&my_chip8, 0x202, 0x7105); // V1 += 0x05 (21)
    
    chip8_emulateCycle(&my_chip8); 
    chip8_emulateCycle(&my_chip8); 
    printf("[6XNN / 7XNN] V1 is now: 0x%02X (Expected: 0x15)\n", my_chip8.V[1]);

    // ----------------------------------------------------
    // TEST 2: Address and Jumps (1NNN, ANNN, BNNN)
    // ----------------------------------------------------
    my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x1250); // JUMP to 0x250
    chip8_emulateCycle(&my_chip8);
    printf("[1NNN] PC after JUMP: 0x%X (Expected: 0x250)\n", my_chip8.pc);

    my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0xA500); // I = 0x500
    chip8_emulateCycle(&my_chip8);
    printf("[ANNN] Register I: 0x%X (Expected: 0x500)\n", my_chip8.I);

    my_chip8.pc = 0x200;
    my_chip8.V[0] = 0x10;
    inject_opcode(&my_chip8, 0x200, 0xB300); // JUMP to 0x300 + V0 (0x310)
    chip8_emulateCycle(&my_chip8);
    printf("[BNNN] PC after JUMP + V0: 0x%X (Expected: 0x310)\n", my_chip8.pc);

    // ----------------------------------------------------
    // TEST 3: Conditional Jumps (3XNN, 4XNN, 5XY0, 9XY0)
    // ----------------------------------------------------
    // 3XNN (Skip if Equal)
    my_chip8.pc = 0x200; my_chip8.V[1] = 0xAA;
    inject_opcode(&my_chip8, 0x200, 0x31AA); // Should skip (PC becomes 0x204)
    chip8_emulateCycle(&my_chip8);
    printf("[3XNN True] PC is now: 0x%X (Expected: 0x204)\n", my_chip8.pc);

    my_chip8.pc = 0x200; my_chip8.V[1] = 0xAA;
    inject_opcode(&my_chip8, 0x200, 0x31BB); // Should NOT skip (PC becomes 0x202)
    chip8_emulateCycle(&my_chip8);
    printf("[3XNN False] PC is now: 0x%X (Expected: 0x202)\n", my_chip8.pc);

    // 4XNN (Skip if Not Equal)
    my_chip8.pc = 0x200; my_chip8.V[1] = 0xAA;
    inject_opcode(&my_chip8, 0x200, 0x41BB); // Should skip (PC becomes 0x204)
    chip8_emulateCycle(&my_chip8);
    printf("[4XNN True] PC is now: 0x%X (Expected: 0x204)\n", my_chip8.pc);

    // 5XY0 (Skip if VX == VY)
    my_chip8.pc = 0x200; my_chip8.V[1] = 0x55; my_chip8.V[2] = 0x55;
    inject_opcode(&my_chip8, 0x200, 0x5120); // Should skip (PC becomes 0x204)
    chip8_emulateCycle(&my_chip8);
    printf("[5XY0 True] PC is now: 0x%X (Expected: 0x204)\n", my_chip8.pc);

    // 9XY0 (Skip if VX != VY)
    my_chip8.pc = 0x200; my_chip8.V[1] = 0x55; my_chip8.V[2] = 0xFF;
    inject_opcode(&my_chip8, 0x200, 0x9120); // Should skip (PC becomes 0x204)
    chip8_emulateCycle(&my_chip8);
    printf("[9XY0 True] PC is now: 0x%X (Expected: 0x204)\n", my_chip8.pc);

    // ----------------------------------------------------
    // TEST 4: The 8-series (Register Arithmetic and Logic)
    // ----------------------------------------------------
    printf("\n--- Testing 8-series ---\n");
    my_chip8.pc = 0x200;
    my_chip8.V[1] = 0x0F; // 0000 1111
    my_chip8.V[2] = 0xF0; // 1111 0000

    // 8XY0: Copy
    inject_opcode(&my_chip8, 0x200, 0x8120); // V1 = V2
    chip8_emulateCycle(&my_chip8);
    printf("[8XY0] V1 after copy: 0x%02X (Expected: 0xF0)\n", my_chip8.V[1]);

    // 8XY1: OR
    my_chip8.V[1] = 0x0F; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8121); // V1 = V1 | V2
    chip8_emulateCycle(&my_chip8);
    printf("[8XY1] V1 after OR: 0x%02X (Expected: 0xFF)\n", my_chip8.V[1]);

    // 8XY2: AND
    my_chip8.V[1] = 0x0F; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8122); // V1 = V1 & V2
    chip8_emulateCycle(&my_chip8);
    printf("[8XY2] V1 after AND: 0x%02X (Expected: 0x00)\n", my_chip8.V[1]);

    // 8XY3: XOR
    my_chip8.V[1] = 0x0F; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8123); // V1 = V1 ^ V2
    chip8_emulateCycle(&my_chip8);
    printf("[8XY3] V1 after XOR: 0x%02X (Expected: 0xFF)\n", my_chip8.V[1]);

    // 8XY4: ADD with Overflow (Carry)
    my_chip8.V[1] = 0xFF; my_chip8.V[2] = 0x02; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8124); // V1 += V2
    chip8_emulateCycle(&my_chip8);
    printf("[8XY4] Overflow - V1: 0x%02X (Expected: 0x01) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY4: ADD without Overflow
    my_chip8.V[1] = 0x10; my_chip8.V[2] = 0x05; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8124);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY4] No overflow - V1: 0x%02X (Expected: 0x15) | VF: %d (Expected: 0)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY5: SUB with underflow (Borrow)
    my_chip8.V[1] = 0x05; my_chip8.V[2] = 0x10; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8125); // V1 = V1 - V2
    chip8_emulateCycle(&my_chip8);
    printf("[8XY5] Underflow - V1: 0x%02X (Expected: 0xF5) | VF: %d (Expected: 0)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY5: SUB without underflow
    my_chip8.V[1] = 0x20; my_chip8.V[2] = 0x05; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8125);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY5] No underflow - V1: 0x%02X (Expected: 0x1B) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY6: Shift Right (LSB was 1)
    my_chip8.V[1] = 0x05; my_chip8.pc = 0x200; 
    inject_opcode(&my_chip8, 0x200, 0x8126);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY6] Shift Right - V1: 0x%02X (Expected: 0x02) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY7: SUBN (V1 = V2 - V1) without underflow
    my_chip8.V[1] = 0x05; my_chip8.V[2] = 0x20; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8127);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY7] SUBN - V1: 0x%02X (Expected: 0x1B) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XYE: Shift Left (MSB was 1)
    my_chip8.V[1] = 0x85; my_chip8.pc = 0x200; 
    inject_opcode(&my_chip8, 0x200, 0x812E);
    chip8_emulateCycle(&my_chip8);
    printf("[8XYE] Shift Left - V1: 0x%02X (Expected: 0x0A) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // ----------------------------------------------------
    // TEST 5: E-Series (Keyboard conditional skips)
    // ----------------------------------------------------
    printf("\n--- Testing E-Series (Keyboard) ---\n");
    // EX9E (Skip if pressed)
    my_chip8.pc = 0x200; my_chip8.V[1] = 5; my_chip8.key[5] = 1; // Key 5 is pressed
    inject_opcode(&my_chip8, 0x200, 0xE19E);
    chip8_emulateCycle(&my_chip8);
    printf("[EX9E True] PC after skip: 0x%X (Expected: 0x204)\n", my_chip8.pc);

    // EXA1 (Skip if NOT pressed)
    my_chip8.pc = 0x200; my_chip8.V[1] = 5; my_chip8.key[5] = 0; // Key 5 is released
    inject_opcode(&my_chip8, 0x200, 0xE1A1);
    chip8_emulateCycle(&my_chip8);
    printf("[EXA1 True] PC after skip: 0x%X (Expected: 0x204)\n", my_chip8.pc);

    // ----------------------------------------------------
    // TEST 6: F-Series - Timers, Sound & Indexes
    // ----------------------------------------------------
    printf("\n--- Testing F-Series (Timers & Indexes) ---\n");
    // FX07: VX = delay_timer
    my_chip8.pc = 0x200; my_chip8.delay_timer = 0x42;
    inject_opcode(&my_chip8, 0x200, 0xF107);
    chip8_emulateCycle(&my_chip8);
    printf("[FX07] V1 loaded from delay_timer: 0x%02X (Expected: 0x42)\n", my_chip8.V[1]);

    // FX15: delay_timer = VX
    my_chip8.pc = 0x200; my_chip8.V[1] = 0x1F;
    inject_opcode(&my_chip8, 0x200, 0xF115);
    chip8_emulateCycle(&my_chip8);
    printf("[FX15] delay_timer set from V1: 0x%02X (Expected: 0x1F)\n", my_chip8.delay_timer);

    // FX18: sound_timer = VX
    my_chip8.pc = 0x200; my_chip8.V[1] = 0x0B;
    inject_opcode(&my_chip8, 0x200, 0xF118);
    chip8_emulateCycle(&my_chip8);
    printf("[FX18] sound_timer set from V1: 0x%02X (Expected: 0x0B)\n", my_chip8.sound_timer);

    // FX1E: I += VX
    my_chip8.pc = 0x200; my_chip8.I = 0x100; my_chip8.V[1] = 0x50;
    inject_opcode(&my_chip8, 0x200, 0xF11E);
    chip8_emulateCycle(&my_chip8);
    printf("[FX1E] Index I after addition: 0x%X (Expected: 0x150)\n", my_chip8.I);

    // FX29: I = font_character_address
    my_chip8.pc = 0x200; my_chip8.V[1] = 2; // Character '2'
    inject_opcode(&my_chip8, 0x200, 0xF129);
    chip8_emulateCycle(&my_chip8);
    printf("[FX29] Index I for character '2': 0x%X (Expected: 0x5A -> 0x50 + (2 * 5))\n", my_chip8.I);

    // ----------------------------------------------------
    // TEST 7: F-Series - FX33 Binary-Coded Decimal (BCD)
    // ----------------------------------------------------
    printf("\n--- Testing FX33 (BCD Conversion) ---\n");
    my_chip8.pc = 0x200; my_chip8.I = 0x300; my_chip8.V[1] = 154; // Hundreds=1, Tens=5, Ones=4
    inject_opcode(&my_chip8, 0x200, 0xF133);
    chip8_emulateCycle(&my_chip8);
    printf("[FX33] Memory[0x300]: %d (Exp: 1) | Memory[0x301]: %d (Exp: 5) | Memory[0x302]: %d (Exp: 4)\n",
            my_chip8.memory[0x300], my_chip8.memory[0x301], my_chip8.memory[0x302]);

    // ----------------------------------------------------
    // TEST 8: F-Series - FX55 / FX65 (Memory Dump & Load)
    // ----------------------------------------------------
    printf("\n--- Testing FX55 / FX65 (Memory Dump & Load) ---\n");

    // FX55: Dump registers V0-V3 into memory starting at I (0x600)
    my_chip8.pc = 0x200; my_chip8.I = 0x600;my_chip8.V[0] = 0x11; my_chip8.V[1] = 0x22; my_chip8.V[2] = 0x33; my_chip8.V[3] = 0x44;
    inject_opcode(&my_chip8, 0x200, 0xF355); // Dump V0 through V3
    chip8_emulateCycle(&my_chip8);
    printf("[FX55] Dumped memory: 0x%02X 0x%02X 0x%02X 0x%02X (Expected: 0x11 0x22 0x33 0x44)\n",
            my_chip8.memory[0x600], my_chip8.memory[0x601], my_chip8.memory[0x602], my_chip8.memory[0x603]);
    
    // FX65: Load registers V0-V3 from memory starting at I (0x600)
    // First we wipe our current registers to see if they reload correctly
    my_chip8.V[0] = 0; my_chip8.V[1] = 0; my_chip8.V[2] = 0; my_chip8.V[3] = 0;my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0xF365); // Load V0 through V3
    chip8_emulateCycle(&my_chip8);printf("[FX65] Loaded registers: V0=0x%02X, V1=0x%02X, V2=0x%02X, V3=0x%02X (Expected: 0x11, 0x22, 0x33, 0x44)\n",
        my_chip8.V[0], my_chip8.V[1], my_chip8.V[2], my_chip8.V[3]);

    // ----------------------------------------------------
    // TEST 9: 0-Series & Stack (00E0, 00EE, 2NNN)
    // ----------------------------------------------------
    printf("\n--- Testing 0-Series & Stack ---\n");

    // 00E0: Clear Screen
    my_chip8.pc = 0x200; my_chip8.gfx[100] = 1; my_chip8.gfx[500] = 1; // Set two pixels
    inject_opcode(&my_chip8, 0x200, 0x00E0);
    chip8_emulateCycle(&my_chip8);
    // Check if the screen was cleared (pixels should be 0)
    printf("[00E0] Wiped screen check: Pixel 100 = %d, Pixel 500 = %d (Expected: 0, 0)\n",my_chip8.gfx[100], my_chip8.gfx[500]);
    
    // 2NNN (Call) & 00EE (Return)
    my_chip8.pc = 0x200; my_chip8.sp = 0;
    inject_opcode(&my_chip8, 0x200, 0x2400); // CALL subroutine at 0x400
    inject_opcode(&my_chip8, 0x400, 0x00EE); // RETURN from subroutine
    chip8_emulateCycle(&my_chip8); // Executes Call (PC goes to 0x400)
    printf("[2NNN Call] PC after CALL: 0x%X (Expected: 0x400) | SP: %d (Expected: 1)\n", 
        my_chip8.pc, my_chip8.sp);chip8_emulateCycle(&my_chip8); // Executes Return (PC should go back to 0x202)
    printf("[00EE Return] PC after RETURN: 0x%X (Expected: 0x202) | SP: %d (Expected: 0)\n", 
        my_chip8.pc, my_chip8.sp);
    
    // ----------------------------------------------------
    // TEST 10: Random Number (CXNN)
    // ----------------------------------------------------
    printf("\n--- Testing Random Number ---\n");
    my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0xC10F); // V1 = rand() & 0x0F
    chip8_emulateCycle(&my_chip8);// With srand(time), this will vary every execution!
    printf("[CXNN] V1 (masked random): 0x%02X (Should vary and end with a value 0x0 to 0x0F)\n", 
        my_chip8.V[1]);
    printf("\n=== ALL AVAILABLE TESTS COMPLETED ===\n");return 0;}
