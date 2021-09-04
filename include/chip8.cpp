/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2021
 */

 #include "chip8.h"

 Chip8::Chip8(bool dumpMemory) {
     memDump = dumpMemory;
 }

 void Chip8::displayStatus() {
     printf("Opcode: %x\n", opcode);
     printf("PC: %x\n", pc);
     printf("SP: %x\n", sp);
     printf("V: ");
     for(int i = 0x0; i < 0xF; ++i) {
         printf("%02x ", V[i]);
     }
     printf("\n");
     printf("I: %x\n", I);
     printf("DT: %x\n", delayTimer);
     printf("ST: %x\n", soundTimer);
     printf("Stack: ");
     for(int i = 0x0; i < 0xF; ++i) {
         printf("%04x ", stack[i]);
     }
     printf("\n");
     if(memDump) {
         printf("Memory: ");
         for(int i = 0x000; i < 0xFFF; ++i) {
             if(i % 5 == 0) {
                 printf("\n");
             }
             printf("%02x", memory[i]);
             printf(" ");
         }
     }
 }

 void Chip8::loadFont() {
     uint8_t font[80] = {
         0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
         0x20, 0x60, 0x20, 0x20, 0x70, // 1
         0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
         0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
         0x90, 0x90, 0xF0, 0x10, 0x10, // 4
         0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
         0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
         0xF0, 0x10, 0x20, 0x40, 0x40, // 7
         0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
         0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
         0xF0, 0x90, 0xF0, 0x90, 0x90, // A
         0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
         0xF0, 0x80, 0x80, 0x80, 0xF0, // C
         0xE0, 0x90, 0x90, 0x90, 0xE0, // D
         0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
         0xF0, 0x80, 0xF0, 0x80, 0x80  // F
     };
     for(int i = 0; i < 80; ++i){
         memory[i+0x050] = font[i];
     }
 }

 void Chip8::init() {
     opcode = 0;
     pc = 0x200;    //ROM data starts at 0x200
     sp = 0;
     I = 0;
     delayTimer = 0;
     soundTimer = 0;

     for(int i = 0; i < 16; ++i) {
         stack[i] = 0;
         V[i] = 0;                  //Clear stack, registers, and keyboard
         keyboard[i] = 0;
     }

     for (int i = 0; i < 0xFFF; ++i) {
         memory[i] = 0;             //Clear memory
     }

     loadFont();
 }

void Chip8::loadRom(std::string romFile) {
    std::ifstream romStream(romFile, std::ios::binary);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(romStream), {});

    if(!romStream.is_open()){
        std::cout << "Error: Failed to open " << romFile << "\n";
    }
    else {
        for(int i = 0; i < buffer.size(); ++i) {
            memory[0x200 + i] = buffer[i];
        }
    }
}

void Chip8::emulateCycle() {
    fetchOpcode();
    CALL_MEMBER_FN(*this, chip8Table[(opcode&0xF000) >> 12]);
}

void Chip8::cpu00E_() {
    switch (opcode & 0x00FF) {
        case 0x00E0:
            cpu00E0();
            break;

        case 0x00EE:
            cpu00EE();
            break;

        default:
            cpuDEFAULT();
            break;
    }
}

void Chip8::cpu00E0() {
    //Clear the display
}

void Chip8::cpu00EE() {
    if(sp <= 0) {
        std::cout << "Stack pointer below 0 at PC: " << pc << std::endl;
        exit(EXIT_FAILURE);
    }
    sp--;
    pc = stack[sp] + 2;
}

void Chip8::cpu1nnn() {
    //TODO
}

void Chip8::cpu2nnn() {
    //TODO
}

void Chip8::cpu3xkk() {
    //TODO
}

void Chip8::cpu4xkk() {
    //TODO
}

void Chip8::cpu5xy0() {
    //TODO
}

void Chip8::cpu6xkk() {
    //TODO
}

void Chip8::cpu7xkk() {
    //TODO
}

void Chip8::cpuARITHMETIC() {
    CALL_MEMBER_FN(*this, chip8Arithmetic[(opcode&0x000F)]);
}

void Chip8::cpu8xy0() {
    //TODO
}

void Chip8::cpu8xy1() {
    //TODO
}

void Chip8::cpu8xy2() {
    //TODO
}

void Chip8::cpu8xy3() {
    //TODO
}

void Chip8::cpu8xy4() {
    //TODO
}

void Chip8::cpu8xy5() {
    //TODO
}

void Chip8::cpu8xy6() {
    //TODO
}

void Chip8::cpu8xy7() {
    //TODO
}

void Chip8::cpu8xyE() {
    //TODO
}

void Chip8::cpu9xy0() {
    //TODO
}

void Chip8::cpuAnnn() {
    //TODO
}

void Chip8::cpuBnnn() {
    //TODO
}

void Chip8::cpuCxkk() {
    //TODO
}

void Chip8::cpuDxyn() {
    //TODO
}

void Chip8::cpuEx_() {
    //TODO
}

void Chip8::cpuEx9E() {
    //TODO
}

void Chip8::cpuExA1() {
    //TODO
}

void Chip8::cpuFx_() {
    switch (opcode & 0x00FF) {
        case 0x0007:
            cpuFx07();
            break;

        case 0x000A:
            cpuFx0A();
            break;

        case 0x0015:
            cpuFx15();
            break;

        case 0x0018:
            cpuFx18();
            break;

        case 0x001E:
            cpuFx1E();
            break;

        case 0x0029:
            cpuFx29();
            break;

        case 0x0033:
            cpuFx33();
            break;

        case 0x0055:
            cpuFx55();
            break;

        case 0x0065:
            cpuFx65();
            break;

        default:
            cpuDEFAULT();
            break;
    }
}

void Chip8::cpuFx07() {
    //TODO
}

void Chip8::cpuFx0A() {
    //TODO
}

void Chip8::cpuFx15() {
    //TODO
}

void Chip8::cpuFx18() {
    //TODO
}

void Chip8::cpuFx1E() {
    //TODO
}

void Chip8::cpuFx29() {
    //TODO
}

void Chip8::cpuFx33() {
    //TODO
}

void Chip8::cpuFx55() {
    //TODO
}

void Chip8::cpuFx65() {
    //TODO
}

void Chip8:: cpuDEFAULT() {
    printf("Bad opcode: %x \n", opcode);
}
