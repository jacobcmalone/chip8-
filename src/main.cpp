/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2021
 */

#include "../include/chip8.h"

#define MEMDUMP false
#define DEBUG true


int main(int argc, char* argv[]) {
    Chip8 chip8(MEMDUMP);

    char wait;
    std::string romFile = "testrom/BD_test.ch8";
    chip8.init();
    chip8.loadRom(romFile);
    while(!chip8.endEmulation()) {
        chip8.emulateCycle();
        if(DEBUG) {
            chip8.displayStatus();
        }
    }
    if(DEBUG) {
        chip8.displayStatus();
    }
    std::cin >> wait;
    return 0;
}
