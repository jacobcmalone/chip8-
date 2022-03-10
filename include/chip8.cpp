/* C++ Chip-8 Interpreter
 * Jacob Malone
 * 2021
 */

 #include "chip8.h"

 Chip8::Chip8(bool dumpMemory, bool wrapX, bool wrapY) {
     memDump = dumpMemory;
     xwrap = wrapX;
     ywrap = wrapY;
 }

 void Chip8::displayStatus() {
     printf("Opcode: %04x\n", opcode);
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
     printf("Screen: \n");
     for(int i = 0; i < 32; ++i) {
         for(int j = 0; j < 64; ++j) {
             printf("%x", screenBuffer[i][j]);
         }
         printf("\n");
     }
     if(memDump) {
         printf("Memory: \n");
         for(int i = 0x000; i < 0xFFF; ++i) {
             if(i % 5 == 0 && i != 0) {
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

     for (int i = 0; i < 32; ++i) {
         for(int j = 0; j < 64; ++j) {
             screenBuffer[i][j] = 0;
         }
     }

     for(int i = 0; i < 16; ++i) {
         stack[i] = 0;
         V[i] = 0;                  //Clear stack, registers, and keyboard
         keyboard[i] = 0;
     }

     for (int i = 0; i < 0xFFF; ++i) {
         memory[i] = 0;             //Clear memory
     }

     srand(time(NULL)); //seed rng

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
        endOfRom = 0x200 + buffer.size();
        printf("End of Rom: %x", endOfRom);
    }
}

void Chip8::emulateCycle() {
    if(endEmulation()) {
        opcode = 0x00E0;
    }
    else
        fetchOpcode();
    std::invoke(chip8Table[(opcode & 0xF000) >> 12], *this);

    //Update timers
    if (delayTimer > 0) {
        --delayTimer;
    }
    if(soundTimer > 0) {
        //TODO: Implement sound
        --soundTimer;
    }
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
    for (int i = 0; i < 32; ++i) {
        for(int j = 0; j < 64; ++j) {
            screenBuffer[i][j] = 0;
        }
    }
    drawFlag = true;
    pc += 2;
}

void Chip8::cpu00EE() {
    if(sp <= 0) {
        std::cout << "Stack pointer below 0 at PC: " << pc << std::endl;
        exit(EXIT_FAILURE);
    }
    sp--;
    pc = stack[sp];
    pc += 2;
}

void Chip8::cpu1nnn() {
    //Jump to address nnn
    pc = (opcode & 0x0FFF);
}

void Chip8::cpu2nnn() {
    //Call subroutine at cpu1nnn
    stack[sp] = pc;
    sp++;
    pc = (opcode & 0x0FFF);
    if(pc < 0x200 || pc > 0xFFF) {
        std::cout << "Out of bounds JMP instruction at PC: " << pc << ", opcode: " << opcode << std::endl;
    }
}

void Chip8::cpu3xkk() {
    //Skips next instruction if Vx == kk
    if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}

void Chip8::cpu4xkk() {
    //Skips next instruction if Vx != kk
    if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}

void Chip8::cpu5xy0() {
    //Skips next instruction if Vx == Vy
    if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}

void Chip8::cpu6xkk() {
    //Set Vx = kk
    V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    pc += 2;
}

void Chip8::cpu7xkk() {
    //Set Vx = Vx + kk
    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    pc += 2;
}

void Chip8::cpuARITHMETIC() {
    std::invoke(chip8Arithmetic[(opcode&0x000F)], *this);
}

void Chip8::cpu8xy0() {
    //Set Vx = Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void Chip8::cpu8xy1() {
    //Set Vx = Vx OR Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void Chip8::cpu8xy2() {
    //Set Vx = Vx AND Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void Chip8::cpu8xy3() {
    //Set Vx = Vx XOR Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void Chip8::cpu8xy4() {
    //Set Vx = Vx + Vy, set VF = carry
    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    pc += 2;
}

void Chip8::cpu8xy5() {
    //Set Vx = Vx - Vy, set VF = NOT borrow
    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void Chip8::cpu8xy6() {
    //Set Vx = Vx SHR 1
    if((V[(opcode & 0x0F00) >> 8] & 1) == 1) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
    pc += 2;
}

void Chip8::cpu8xy7() {
    //Set Vx = Vy - Vx, set VF = NOT borrow
    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void Chip8::cpu8xyE() {
    //Set Vx = Vx SHL 1
    if(V[(opcode & 0x0F00) >> 8] & 0x80 == 1) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
    pc += 2;
}

void Chip8::cpu9xy0() {
    //Skips next instruction if Vx != Vy
    if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}

void Chip8::cpuAnnn() {
    //Set I = nnn
    I = opcode & 0x0FFF;
    pc += 2;
}

void Chip8::cpuBnnn() {
    //Jump to location nnn + V0
    pc = (opcode & 0x0FFF) + V[0];
}

void Chip8::cpuCxkk() {
    //Set Vx = random byte AND kk
    V[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
    pc += 2;
}

void Chip8::cpuDxyn() {
    //Display n-byte sprite starting at memory location I at (Vx, Vy)
    //Set VF = collision
    uint8_t n = opcode & 0x000F;
    uint8_t x = V[(opcode & 0x0F00) >> 8];
    uint8_t y = V[(opcode & 0x00F0) >> 4];
    uint8_t sprite;
    V[0xF] = 0;

    for (uint8_t yOffset = 0; yOffset < n; ++yOffset) {
        sprite = memory[I + yOffset];
        for (uint8_t xOffset = 0; xOffset < 8; ++xOffset) {
            if(screenBuffer[y+yOffset][x+xOffset] & ((sprite >> (7-xOffset)) & 1) != 0) {
                V[0xF] = 1;
            }
            screenBuffer[y+yOffset][x+xOffset] ^= ((sprite >> (7-xOffset)) & 1);
        }
    }

    drawFlag = true;
    pc += 2;
}

void Chip8::cpuEx_() {
    switch (opcode & 0x00FF) {
        case 0x009E:
            cpuEx9E();
            break;

        case 0x00A1:
            cpuExA1();
            break;

        default:
            cpuDEFAULT();
            break;
    }
}

void Chip8::cpuEx9E() {
    //Skips next instruction if key w/ value Vx is pressed
    if(keyboard[V[(opcode & 0x0F00) >> 8]] != 0) {
        pc += 4;
    }
    else {
        pc += 2;
    }
}

void Chip8::cpuExA1() {
    //Skips next instruction if key w/ value Vx is NOT pressed
    if(keyboard[V[(opcode & 0x0F00) >> 8]] == 0) {
        pc += 4;
    }
    else {
        pc += 2;
    }
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
    //TSet Vx = delayTimer
    V[(opcode & 0x0F00) >> 8] = delayTimer;
    pc +=2;
}

void Chip8::cpuFx0A() {
    //Wait for key press, then store the value of the key in Vx
    bool key_pressed = false;
    while (!key_pressed) {
        for (int i = 0; i < 16; ++i) {
            if(keyboard[i] != 0) {
                V[(opcode & 0x0F00) >> 8] = i;
                key_pressed = true;
                break;
            }
        }
    }
    pc += 2;
}

void Chip8::cpuFx15() {
    //Set delayTimer = Vx
    delayTimer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void Chip8::cpuFx18() {
    //Set soundTimer = Vx
    soundTimer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void Chip8::cpuFx1E() {
    //Set I = I + Vx
    I += V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void Chip8::cpuFx29() {
    //Set I = location of sprite for digit Vx
    I = V[(opcode & 0x0F00) >> 8] * 0x5;
    pc += 2;
}

void Chip8::cpuFx33() {
    //Store BCD representation of Vx in memory locations I, I+1, I+2
    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;

    pc += 2;
}

void Chip8::cpuFx55() {
    //Store registers V0 through Vx in memory starting at location I.
    int x = (opcode & 0x0F00) >> 8;
    for (int j = 0; j <= x; ++j) {
        memory[I + j] = V[j];
    }
    pc += 2;
}

void Chip8::cpuFx65() {
    //Read registers V0 through Vx from memory starting at location I.
    int x = (opcode & 0x0F00) >> 8;
    for (int j = 0; j <= x; ++j) {
        V[j] = memory[I + j];
    }
    pc += 2;
}

void Chip8:: cpuDEFAULT() {
    printf("Bad opcode: %x \n", opcode);
}
