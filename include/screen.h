/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2022
 */

 #include <SDL2/SDL.h>
 #include <cstdio>

 class Screen {
    public:
        Screen(SDL_Window* w = NULL, SDL_Surface* s = NULL, SDL_Surface* f = NULL);
        void init();
        void draw(uint8_t screenBuffer[32][64]);
        void close();
    private:
        SDL_Window* window;
        SDL_Surface* surface;
        SDL_Surface* frameBuffer;
        const int SCALING = 8;
        const uint32_t BG = 0x0;
        const uint32_t FG = 0xFFFFFF;
 };
