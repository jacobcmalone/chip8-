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

const uint32_t frameTime = 2;

int main (int argc, char* argv[]) {

    if(argc < 2) {
        std::cout << "Usage: ./chip8.exe [path to ROM]\n";
        return 0;
    }

    const std::string romFile = argv[1];

    Chip8 chip8(MEMDUMP);
    chip8.init();
    chip8.loadRom(romFile);

    Screen screen;
    screen.init();

    uint32_t start_time;
    uint32_t last_time;
    uint32_t elapsed_time;

    bool quit = false;
    last_time = SDL_GetTicks();

    while(!quit) {
        start_time = SDL_GetTicks();
        elapsed_time = start_time - last_time;

        //Handle SDL Events (Keyboard)
        screen.handleInput(chip8.keyboard);

        chip8.emulateCycle();
        if(chip8.endEmulation())
            quit = true;

        //Draw to SDL window
        if(chip8.drawFlag){
            chip8.drawFlag = false;
            screen.draw(chip8.screenBuffer);
        }

        if(DEBUG) {
            chip8.displayStatus();
        }

        if(SDL_GetTicks() - start_time < frameTime) {
            SDL_Delay(frameTime - (SDL_GetTicks() - start_time));
        }

    }
    screen.close();
    return 0;
}
