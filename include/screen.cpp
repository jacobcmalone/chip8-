/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2022
 */

 #include "screen.h"

 Screen::Screen(SDL_Window* w, SDL_Surface* s, SDL_Surface* f) {
     window = w;
     surface = s;
     frameBuffer = f;
 }

 void Screen::init() {
     if (SDL_Init(SDL_INIT_VIDEO) > 0) {
         printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
     }
     else {
         window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED, 64 * SCALING,
                                 32 * SCALING, SDL_WINDOW_SHOWN);
         if (window == NULL) {
             printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
         }
         else {
             surface = SDL_GetWindowSurface(window);
             frameBuffer = SDL_GetWindowSurface(window);
         }
     }
 }

 void Screen::draw(uint8_t screenBuffer[32][64]) {
     SDL_FillRect(frameBuffer, NULL, BG);
     SDL_Rect pixel;
     for(int y = 0; y < 32; ++y) {
         for(int x = 0; x < 64; ++x) {
             if(screenBuffer[y][x] == 1) {
                 pixel.x = x * SCALING;
                 pixel.y = y * SCALING;
                 pixel.w = pixel.h = SCALING;
                 SDL_FillRect(frameBuffer, &pixel, FG);
             }
         }
     }
     SDL_BlitSurface(frameBuffer, NULL, surface, NULL);
     SDL_UpdateWindowSurface(window);
 }

 void Screen::close() {
     SDL_FreeSurface(frameBuffer);
     SDL_DestroyWindow(window);
     SDL_Quit();
 }
