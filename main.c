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

RomReader my_reader;
Chip8 my_chip8;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int main(int argc, char* argv[]) {
    printf("=== Starting CHIP-8 Emulator with SDL2 ===\n");

    my_reader.filename = "Pong.ch8";

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
    uint32_t last_timer_time = SDL_GetTicks();

    // Setting up the emulator core
    chip8_initialize(&my_chip8);

    // Read ROM file
    rom_reader(&my_reader);

    // Load ROM into memory
    rom_loader(&my_chip8, &my_reader);

    while(running) {

        // Check the OS mailbox for any user interactions (inputs, clicks, closes)
        while (SDL_PollEvent(&event)) {
            // If the user clicks the X button on the window, it gets closed
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            // If a key is pressed
            else if (event.type == SDL_KEYDOWN) {
                char key = event.key.keysym.sym;
                handle_key_input(&my_chip8, key, 1);
            }
            // If a key is released
            else if (event.type == SDL_KEYUP) {
                char key = event.key.keysym.sym;
                handle_key_input(&my_chip8, key, 0);
            }
        }
    
    chip8_emulateCycle(&my_chip8);
    // 60 Hz Timer logic: Check if 16.6 milliseconds have passed
    if (SDL_GetTicks() - last_timer_time >= 16) {
        chip8_update_timers(&my_chip8); 
        last_timer_time = SDL_GetTicks(); // Reset the stopwatch
    }
    SDL_Delay(1);

    }
    // Clean up memory and close SDL2 before the program terminates
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("=== Program ended cleanly ===\n");
    return 0;
}