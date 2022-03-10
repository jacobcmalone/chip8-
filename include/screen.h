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
        void handleInput(int chip8keyboard[16]);
        void close();
    private:
        SDL_Window* window;
        SDL_Surface* surface;
        SDL_Surface* frameBuffer;
        SDL_Event e;
        const int SCALING = 8;
        const uint32_t BG = 0x0;
        const uint32_t FG = 0xFFFFFF;
        int keys[16] = {
            SDLK_x, SDLK_1, SDLK_2, SDLK_3,
            SDLK_q, SDLK_w, SDLK_e, SDLK_a,
            SDLK_s, SDLK_d, SDLK_z, SDLK_c,
            SDLK_4, SDLK_r, SDLK_f, SDLK_v
        };
 };
