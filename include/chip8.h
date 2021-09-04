/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2021
 */

 #include <cstdint>
 #include <cstdio>
 #include <string>
 #include <fstream>
 #include <iostream>
 #include <vector>

//This macro is for an array of pointer-to-member-function
 #define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember))

 class Chip8 {
    typedef void (Chip8::*Chip8MemFn)();    //Pointer-to-member-function typedef
    public:
        Chip8(bool memoryDump = false);
        void displayStatus();
        void init();
        void loadRom(std::string romFile);
        void emulateCycle();

        uint8_t screen[64*32];
        uint8_t keyboard[16];
        bool drawFlag;

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

        Chip8MemFn chip8Table[17] = {
            cpu00E_, cpu1nnn, cpu2nnn, cpu3xkk, cpu4xkk, cpu5xy0, cpu6xkk, cpu7xkk,
            cpuARITHMETIC, cpu9xy0, cpuAnnn, cpuBnnn, cpuCxkk, cpuDxyn, cpuEx_, cpuFx_,
            cpuDEFAULT
        };

        Chip8MemFn chip8Arithmetic[16] = {
            cpu8xy0, cpu8xy1, cpu8xy2, cpu8xy3, cpu8xy4, cpu8xy5, cpu8xy6, cpu8xy7,
            cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpu8xyE, cpuDEFAULT,
        };

        //Chip8MemFn chip8Ex_[3] = {
        //    cpuEx9E, cpuExA1, cpuDEFAULT
        //}

        //Chip8MemFn chip8Fx_[10] = {
        //    cpuFx07, cpuFx0A, cpuFx15, cpuFx18, cpuFx1E,
        //    cpuFx29, cpuFx33, cpuFx55, cpuFx65, cpuDEFAULT
        //}

};
