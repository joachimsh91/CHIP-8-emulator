#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

// The indexes (0-15) represent the CHIP-8 keys.
// The values represent the characters on your PC keyboard.
const char KEYMAP[16] = {
    '1', '2', '3', '4',   // CHIP-8: 0, 1, 2, 3
    'q', 'w', 'e', 'r',   // CHIP-8: 4, 5, 6, 7
    'a', 's', 'd', 'f',   // CHIP-8: 8, 9, A, B
    'z', 'x', 'c', 'v'    // CHIP-8: C, D, E, F
};

void handle_key_input(Chip8* system, char key, int is_pressed) { // Keyboard handler
    for (int i = 0; i < 16; i++) {
        if (key == KEYMAP[i]) {
            system->key[i] = is_pressed;
            break;
        }
    }
}