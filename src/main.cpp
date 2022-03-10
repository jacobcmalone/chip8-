/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2021
 */

#include "chip8.h"
#include "screen.h"
#include <chrono>
#include <thread>

#define MEMDUMP false
#define DEBUG false

const std::string romFile = "roms/space_invaders.ch8";
/*
const int SCALING = 8;
const uint32_t BG = 0x0;
const uint32_t FG = 0xFFFFFF;
*/
const uint32_t frameTime = 2;

int keys[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

int main (int argc, char* argv[]) {

    Chip8 chip8(MEMDUMP);
    chip8.init();
    chip8.loadRom(romFile);

    Screen screen;
    screen.init();

    uint32_t start_time;
    uint32_t last_time;
    uint32_t elapsed_time;

    /*
    SDL_Window* window = NULL;
    SDL_Surface* surface = NULL;
    SDL_Surface* frameBuffer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) > 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else {
        window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, 64 * SCALING,
                                32 * SCALING, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 1;
        }
        else {
            surface = SDL_GetWindowSurface(window);
            frameBuffer = SDL_GetWindowSurface(window);
        }
    }
    */

    bool quit = false;
    SDL_Event e;
    last_time = SDL_GetTicks();

    while(!quit) {
        start_time = SDL_GetTicks();
        elapsed_time = start_time - last_time;

        //Handle SDL Events (Keyboard)
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                return 0;
            }
            if(e.type == SDL_KEYDOWN) {
                if(e.key.keysym.sym == SDLK_ESCAPE) {
                    return 0;
                }
                for(int i = 0; i < 16; ++i) {
                    if(e.key.keysym.sym == keys[i]) {
                        chip8.keyboard[i] = 1;
                    }
                }

            }
            if(e.type == SDL_KEYUP) {
                for(int i = 0; i < 16; ++i) {
                    if(e.key.keysym.sym == keys[i]) {
                        chip8.keyboard[i] = 0;
                    }
                }

            }
        }

        chip8.emulateCycle();
        if(chip8.endEmulation())
            quit = true;

        //Draw to SDL window
        if(chip8.drawFlag){
            chip8.drawFlag = false;
            screen.draw(chip8.screenBuffer);
            /*
            SDL_FillRect(frameBuffer, NULL, BG);
            SDL_Rect pixel;
            for(int y = 0; y < 32; ++y) {
                for(int x = 0; x < 64; ++x) {
                    if(chip8.screenBuffer[y][x] == 1) {
                        pixel.x = x * SCALING;
                        pixel.y = y * SCALING;
                        pixel.w = pixel.h = SCALING;
                        SDL_FillRect(frameBuffer, &pixel, FG);
                    }
                }
            }
            SDL_BlitSurface(frameBuffer, NULL, surface, NULL);
            SDL_UpdateWindowSurface(window);
            */
        }

        if(DEBUG) {
            chip8.displayStatus();
        }

        if(SDL_GetTicks() - start_time < frameTime) {
            SDL_Delay(frameTime - (SDL_GetTicks() - start_time));
        }

    }
    /*
    SDL_FreeSurface(frameBuffer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    */
    screen.close();
    return 0;
}
