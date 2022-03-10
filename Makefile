all:
	g++ -Iinclude -Iinclude/SDL2  -Linclude/lib -o chip8 src/main.cpp include/chip8.cpp include/screen.cpp -lcygwin -lSDL2main -lSDL2
