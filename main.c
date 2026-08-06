#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"
#include <SDL2/SDL.h>

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

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int main(int argc, char* argv[]) {
    printf("=== Starting CHIP-8 Emulator with SDL2 ===\n");

    // Initialize SDL2 (We request both video/graphics and audio/sound!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create the window on your PC
    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulator",                  // Window title
        SDL_WINDOWPOS_UNDEFINED,            // Start position X on the screen
        SDL_WINDOWPOS_UNDEFINED,            // Start position Y on the screen
        WINDOW_WIDTH, WINDOW_HEIGHT,        // Width and height in pixels
        SDL_WINDOW_SHOWN                    // Show the window immediately
    );

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    printf("SDL2 initialized and window created successfully!\n");

    int running = 1;
    SDL_Event event;
    while(running) {

        // Check the OS mailbox for any user interactions (inputs, clicks, closes)
        while (SDL_PollEvent(&event)) {
            // If the user clicks the X button on the window, exit neatly
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_Delay(1);
    }

    // Clean up memory and close SDL2 before the program terminates
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("=== Program ended cleanly ===\n");
    return 0;
}