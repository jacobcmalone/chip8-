/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2021
 */

#include <chip8.h>
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

#define MEMDUMP false
#define DEBUG false

const std::string romFile = "roms/space_invaders.ch8";
const int SCALING = 8;
const uint32_t BG = 0x0;
const uint32_t FG = 0xFFFFFF;
const uint32_t frameTime = 2;

int keys[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

//void processKeyDown(SDL_Event keydown);
//void processKeyUp(SDL_Event keyup);

int main (int argc, char* argv[]) {

    Chip8 chip8(MEMDUMP);
    chip8.init();
    chip8.loadRom(romFile);

    uint32_t start_time;
    uint32_t last_time;
    uint32_t elapsed_time;

    SDL_Window* window = NULL;
    SDL_Surface* surface = NULL;

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
        }
    }

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
                /*
                switch(e.key.keysym.sym) {
                    case SDLK_x:
                        chip8.keyboard[0] = 1;
                        break;
                    case SDLK_1:
                        chip8.keyboard[1] = 1;
                        break;
                    case SDLK_2:
                        chip8.keyboard[2] = 1;
                        break;
                    case SDLK_3:
                        chip8.keyboard[3] = 1;
                        break;
                    case SDLK_q:
                        chip8.keyboard[4] = 1;
                        break;
                    case SDLK_w:
                        chip8.keyboard[5] = 1;
                        break;
                    case SDLK_e:
                        chip8.keyboard[6] = 1;
                        break;
                    case SDLK_a:
                        chip8.keyboard[7] = 1;
                        break;
                    case SDLK_s:
                        chip8.keyboard[8] = 1;
                        break;
                    case SDLK_d:
                        chip8.keyboard[9] = 1;
                        break;
                    case SDLK_z:
                        chip8.keyboard[0xA] = 1;
                        break;
                    case SDLK_c:
                        chip8.keyboard[0xB] = 1;
                        break;
                    case SDLK_4:
                        chip8.keyboard[0xC] = 1;
                        break;
                    case SDLK_r:
                        chip8.keyboard[0xD] = 1;
                        break;
                    case SDLK_f:
                        chip8.keyboard[0xE] = 1;
                        break;
                    case SDLK_v:
                        chip8.keyboard[0xF] = 1;
                        break;
                    default:
                        break;
                }
                */
                
                for(int i = 0; i < 16; ++i) {
                    if(e.key.keysym.sym == keys[i]) {
                        chip8.keyboard[i] = 1;
                    }
                }

            }
            if(e.type == SDL_KEYUP) {
                /*
                switch(e.key.keysym.sym) {
                    case SDLK_x:
                        chip8.keyboard[0] = 0;
                        break;
                    case SDLK_1:
                        chip8.keyboard[1] = 0;
                        break;
                    case SDLK_2:
                        chip8.keyboard[2] = 0;
                        break;
                    case SDLK_3:
                        chip8.keyboard[3] = 0;
                        break;
                    case SDLK_q:
                        chip8.keyboard[4] = 0;
                        break;
                    case SDLK_w:
                        chip8.keyboard[5] = 0;
                        break;
                    case SDLK_e:
                        chip8.keyboard[6] = 0;
                        break;
                    case SDLK_a:
                        chip8.keyboard[7] = 0;
                        break;
                    case SDLK_s:
                        chip8.keyboard[8] = 0;
                        break;
                    case SDLK_d:
                        chip8.keyboard[9] = 0;
                        break;
                    case SDLK_z:
                        chip8.keyboard[0xA] = 0;
                        break;
                    case SDLK_c:
                        chip8.keyboard[0xB] = 0;
                        break;
                    case SDLK_4:
                        chip8.keyboard[0xC] = 0;
                        break;
                    case SDLK_r:
                        chip8.keyboard[0xD] = 0;
                        break;
                    case SDLK_f:
                        chip8.keyboard[0xE] = 0;
                        break;
                    case SDLK_v:
                        chip8.keyboard[0xF] = 0;
                        break;
                    default:
                        break;
                }
                */

                for(int i = 0; i < 16; ++i) {
                    if(e.key.keysym.sym == keys[i]) {
                        chip8.keyboard[i] = 0;
                    }
                }

            }
        }

        chip8.emulateCycle();

        //Draw to SDL window
        if(chip8.drawFlag){
            chip8.drawFlag = false;
            SDL_FillRect(surface, NULL, BG);
            SDL_Rect pixel;
            for(int y = 0; y < 32; ++y) {
                for(int x = 0; x < 64; ++x) {
                    if(chip8.screenBuffer[y][x] == 1) {
                        pixel.x = x * SCALING;
                        pixel.y = y * SCALING;
                        pixel.w = pixel.h = SCALING;
                        SDL_FillRect(surface, &pixel, FG);
                    }
                }
            }
            SDL_UpdateWindowSurface(window);
        }

        if(DEBUG) {
            chip8.displayStatus();
        }

        if(SDL_GetTicks() - start_time < frameTime) {
            SDL_Delay(frameTime - (SDL_GetTicks() - start_time));
        }

        last_time = start_time;
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
