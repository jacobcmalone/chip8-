/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2021
 */

 #include <cstdint>
 #include <cstdio>
 #include <cstdlib>
 #include <time.h>
 #include <string>
 #include <fstream>
 #include <iostream>
 #include <vector>
 #include <random>
 #include <functional>

//This macro is for an array of pointer-to-member-function
//#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember))

 class Chip8 {
    public:
        Chip8(bool memoryDump = false, bool wrapX = true, bool wrapY = true);
        void displayStatus();
        void init();
        void loadRom(std::string romFile);
        void emulateCycle();
        bool endEmulation() {return pc >= endOfRom;}

        uint8_t screenBuffer[32][64]; //bitmap 64 * 32
        uint8_t keyboard[16];
        bool drawFlag;

        typedef void (Chip8::*Chip8MemFn)();    //Pointer-to-member-function typedef
        Chip8MemFn chip8Table[16] = {
            &Chip8::cpu00E_, &Chip8::cpu1nnn, &Chip8::cpu2nnn, &Chip8::cpu3xkk,
            &Chip8::cpu4xkk, &Chip8::cpu5xy0, &Chip8::cpu6xkk, &Chip8::cpu7xkk,
            &Chip8::cpuARITHMETIC, &Chip8::cpu9xy0, &Chip8::cpuAnnn, &Chip8::cpuBnnn,
            &Chip8::cpuCxkk, &Chip8::cpuDxyn, &Chip8::cpuEx_, &Chip8::cpuFx_
        };

        Chip8MemFn chip8Arithmetic[16] = {
            &Chip8::cpu8xy0, &Chip8::cpu8xy1, &Chip8::cpu8xy2, &Chip8::cpu8xy3,
            &Chip8::cpu8xy4, &Chip8::cpu8xy5, &Chip8::cpu8xy6, &Chip8::cpu8xy7,
            &Chip8::cpuNULL, &Chip8::cpuNULL, &Chip8::cpuNULL, &Chip8::cpuNULL,
            &Chip8::cpuNULL, &Chip8::cpuNULL, &Chip8::cpu8xyE, &Chip8::cpuDEFAULT
        };



    private:
        uint8_t memory[0xFFF];
        uint16_t opcode;
        uint16_t pc;             //Program Counter
        uint8_t sp;              //Stack Pointer, points to topmost level of stack
        uint8_t V[16];           //General Purpose Registers
        uint16_t I;              //Used to store memory addresses
        uint16_t stack[16];
        uint8_t delayTimer;
        uint8_t soundTimer;
        bool memDump;
        bool xwrap;
        bool ywrap;
        uint16_t endOfRom;

        void loadFont();

        void fetchOpcode() {
            opcode = memory[pc] << 8 | memory[pc + 1];
        }

        /*
         *  From Cowgod's Chip-8 Technical Reference
         *  http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
         *
         *  In these listings, the following variables are used:
         *
         *  nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
         *  n or nibble - A 4-bit value, the lowest 4 bits of the instruction
         *  x - A 4-bit value, the lower 4 bits of the high byte of the instruction
         *  y - A 4-bit value, the upper 4 bits of the low byte of the instruction
         *  kk or byte - An 8-bit value, the lowest 8 bits of the instruction
         */

        void cpuNULL() {}

        void cpu00E_();
        void cpu00E0();         //CLS
        void cpu00EE();         //RET

        void cpu1nnn();         //JP   addr
        void cpu2nnn();         //CALL addr
        void cpu3xkk();         //SE  Vx, byte
        void cpu4xkk();         //SNE Vx, byte
        void cpu5xy0();         //SE  Vx, Vy
        void cpu6xkk();         //LD  Vx, byte
        void cpu7xkk();         //ADD Vx, byte

        void cpuARITHMETIC();
        void cpu8xy0();         //LD   Vx, Vy
        void cpu8xy1();         //OR   Vx, Vy
        void cpu8xy2();         //AND  Vx, Vy
        void cpu8xy3();         //XOR  Vx, Vy
        void cpu8xy4();         //ADD  Vx, Vy
        void cpu8xy5();         //SUB  Vx, Vy
        void cpu8xy6();         //SHR  Vx {, Vy}
        void cpu8xy7();         //SUBN Vx, Vy
        void cpu8xyE();         //SHL  Vx {, Vy}

        void cpu9xy0();         //SNE Vx, Vy
        void cpuAnnn();         //LD  I,  addr
        void cpuBnnn();         //JP  V0, addr
        void cpuCxkk();         //RND Vx, byte
        void cpuDxyn();         //DRW Vx, Vy, nibble

        void cpuEx_();
        void cpuEx9E();         //SKP  Vx
        void cpuExA1();         //SKNP Vx

        void cpuFx_();
        void cpuFx07();         //LD  Vx,  DT
        void cpuFx0A();         //LD  Vx,  K
        void cpuFx15();         //LD  DT,  Vx
        void cpuFx18();         //LD  ST,  Vx
        void cpuFx1E();         //ADD I,   Vx
        void cpuFx29();         //LD  F,   Vx
        void cpuFx33();         //LD  B,   Vx
        void cpuFx55();         //LD  [I], Vx
        void cpuFx65();         //LD  Vx,  [I]

        void cpuDEFAULT();

};
