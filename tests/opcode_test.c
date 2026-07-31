#include <stdio.h>
#include <stdlib.h>
#include "../chip8.h"

// Helper function to inject a 16-bit opcode into memory at a specific address
void inject_opcode(Chip8* system, uint16_t addr, uint16_t opcode) {
    system->memory[addr] = (opcode & 0xFF00) >> 8;
    system->memory[addr + 1] = opcode & 0x00FF;
}

int main() {
    Chip8 my_chip8;
    chip8_initialize(&my_chip8);
    
    printf("=== STARTING COMPREHENSIVE OPCODE TEST ===\n\n");

    // ----------------------------------------------------
    // TEST 1: Core assignment and arithmetic (6XNN, 7XNN)
    // ----------------------------------------------------
    my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x6110); // V1 = 0x10 (16)
    inject_opcode(&my_chip8, 0x202, 0x7105); // V1 += 0x05 (21)
    
    chip8_emulateCycle(&my_chip8); // Executes 0x6110
    chip8_emulateCycle(&my_chip8); // Executes 0x7105
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
    inject_opcode(&my_chip8, 0x200, 0x8124); // V1 += V2 (255 + 2 = 257 -> rolls over to 1)
    chip8_emulateCycle(&my_chip8);
    printf("[8XY4] Overflow - V1: 0x%02X (Expected: 0x01) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY4: ADD without Overflow
    my_chip8.V[1] = 0x10; my_chip8.V[2] = 0x05; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8124);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY4] No overflow - V1: 0x%02X (Expected: 0x15) | VF: %d (Expected: 0)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY5: SUB with underflow (Borrow)
    my_chip8.V[1] = 0x05; my_chip8.V[2] = 0x10; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8125); // V1 = V1 - V2 (5 - 16 -> underflow)
    chip8_emulateCycle(&my_chip8);
    printf("[8XY5] Underflow - V1: 0x%02X (Expected: 0xF5) | VF: %d (Expected: 0)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY5: SUB without underflow
    my_chip8.V[1] = 0x20; my_chip8.V[2] = 0x05; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8125);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY5] No underflow - V1: 0x%02X (Expected: 0x1B) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY6: Shift Right (LSB was 1)
    my_chip8.V[1] = 0x05; my_chip8.pc = 0x200; // 5 = binary 00000101 (LSB = 1)
    inject_opcode(&my_chip8, 0x200, 0x8126);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY6] Shift Right - V1: 0x%02X (Expected: 0x02) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XY7: SUBN (V1 = V2 - V1) without underflow
    my_chip8.V[1] = 0x05; my_chip8.V[2] = 0x20; my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0x8127);
    chip8_emulateCycle(&my_chip8);
    printf("[8XY7] SUBN - V1: 0x%02X (Expected: 0x1B) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // 8XYE: Shift Left (MSB was 1)
    my_chip8.V[1] = 0x85; my_chip8.pc = 0x200; // 0x85 = binary 10000101 (MSB = 1)
    inject_opcode(&my_chip8, 0x200, 0x812E);
    chip8_emulateCycle(&my_chip8);
    printf("[8XYE] Shift Left - V1: 0x%02X (Expected: 0x0A) | VF: %d (Expected: 1)\n", my_chip8.V[1], my_chip8.V[0xF]);

    // ----------------------------------------------------
    // TEST 5: Stack and Subroutine Calls (2NNN)
    // ----------------------------------------------------
    printf("\n--- Testing Stack ---\n");
    my_chip8.pc = 0x200;
    my_chip8.sp = 0;
    inject_opcode(&my_chip8, 0x200, 0x2400); // CALL subroutine at address 0x400
    chip8_emulateCycle(&my_chip8);
    printf("[2NNN] PC after CALL: 0x%X (Expected: 0x400) | SP: %d (Expected: 1) | Stack[0]: 0x%X (Expected: 0x202)\n", 
            my_chip8.pc, my_chip8.sp, my_chip8.stack[0]);

    // ----------------------------------------------------
    // TEST 6: Random Number (CXNN)
    // ----------------------------------------------------
    printf("\n--- Testing Random Number ---\n");
    my_chip8.pc = 0x200;
    inject_opcode(&my_chip8, 0x200, 0xC10F); // V1 = rand() & 0x0F (Result MUST end in something between 0x0 and 0x0F)
    chip8_emulateCycle(&my_chip8);
    printf("[CXNN] V1 (masked random): 0x%02X (Should be a number where the first digit is 0, e.g., 0x03, 0x0A, etc.)\n", my_chip8.V[1]);

    printf("\n=== TEST COMPLETED ===\n");
    return 0;
}

