#include <fstream>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include "chip8.h"

unsigned char font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void (chip8::* chip8::opcodeTable[numOpFns])(unsigned short) {
    #define X(name) &chip8::name,
        OPCODE_FNS
    #undef X
};

chip8::chip8() {
    // Reset Vars
    PC = 0x200;
    I = 0;
    sp = 0;
    drawFlag = false;
    soundFlag = false;
    delayTimer = 0;
    soundTimer = 0;
    prevTime = getTime();

    // Reset registers, stack, and keys
    for (int i = 0; i < constants::NUM_KEYS; i++) {
        V[i] = 0;
        stack[i] = 0;
        key[i] = 0;
    }

    // Reset memory
    for (int i = 0; i < constants::MEM_LOCATIONS; i++) {
        mem[i] = 0;
    }

    // Reset Screen
    for (int i = 0; i < constants::RES_H; i++) {
        for (int j = 0; j < constants::RES_W; j++) {
            screen[i][j] = 0;
        }
    }

    // Load font into memory
    for (int i = 0; i < 80; i++) {
        mem[i] = font[i];
    }
}

chip8::~chip8() {
	// Nothing
}

/*
 * Function loadFile()
 * Takes file path in as a variable
 * Returns true if loaded successfully, false otherwise
 * Loads the rom data into memory starting at 0x200
 */
bool chip8::loadFile(std::string path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Could not open " << path << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size{ file.tellg() };
    if (size > constants::MEM_LOCATIONS - constants::START_ADR) {
        std::cout << path << " is too large\n";
        return false;
    }

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&mem[constants::START_ADR]), size);

    return true;
}

/*
 * Function runCycle()
 * Takes no parameters
 * Returns no value
 * Simulates one chip cycle: Fetch -> Decode -> Execute -> Update Timers
 * Opcodes are stored
 */
void chip8::runCycle() {
    // Opcodes are stored in 2 memory locations using big-endian (mem[PC] is first half and mem[PC + 1] is second half)
    unsigned short opcode = (mem[PC] << 8) | (mem[PC + 1]);
    PC += 2;

    // Note: 0NNN is not implemented on this system
    switch (opcode >> 12) {
    case 0x0:
        switch (opcode) {
        case 0x00E0: // 00E0: Clears the screen
            for (int i = 0; i < constants::RES_H; i++) {
                for (int j = 0; j < constants::RES_W; j++) {
                    screen[i][j] = 0;
                }
            }
            break;

        case 0x00EE: // 00EE: Returns from subroutine
            sp--;
            PC = stack[sp];
        }
        break;
    case 0x1: // 1NNN: Jumps to address NNN
        PC = opcode & 0xFFF;
        break;

    case 0x2: // 2NNN: Calls subroutine at NNN
        stack[sp] = PC; // 2NNN: Calls subroutine at NNN
        sp++;
        PC = opcode & 0xFFF;
        break;

    case 0x3: // 3XNN: Skips the next instruction if VX equals NN 
        if (V[opcode >> 8 & 0xF] == (opcode & 0xFF)) { // 3XNN: Skips the next instruction if VX equals NN 
            PC += 2;
        }
        break;

    case 0x4: // 4XNN: Skips the next instruction if VX does not equal NN
        if (V[opcode >> 8 & 0xF] != (opcode & 0xFF)) { // 4XNN: Skips the next instruction if VX does not equal NN
            PC += 2;
        }
        break;

    case 0x5: // 5XY0: Skips the next instruction if VX equals VY
        if (V[opcode >> 8 & 0xF] == V[opcode >> 4 & 0xF]) { // 5XY0: Skips the next instruction if VX equals VY
            PC += 2;
        }
        break;

    case 0x6: // 6XNN: Sets VX to NN
        V[opcode >> 8 & 0xF] = opcode & 0xFF; // 6XNN: Sets VX to NN
        break;

    case 0x7: // 7XNN: Adds NN to VX (carry flag is not changed)
        V[opcode >> 8 & 0xF] += opcode & 0xFF; // 7XNN: Adds NN to VX (carry flag is not changed)
        break;

    case 0x8:
        switch (opcode & 0xF) {
        case 0x0: // 8XY0: Sets VX to the value of VY
            V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF];
            break;
        case 0x1: // 8XY1: Sets VX to VX or VY
            V[opcode >> 8 & 0xF] |= V[opcode >> 4 & 0xF];
            V[0xF] = 0;
            break;
        case 0x2: // 8XY2: Sets VX to VX and VY
            V[opcode >> 8 & 0xF] &= V[opcode >> 4 & 0xF];
            V[0xF] = 0;
            break;
        case 0x3: // 8XY3: Sets VX to VX xor VY
            V[opcode >> 8 & 0xF] ^= V[opcode >> 4 & 0xF];
            V[0xF] = 0;
            break;
        case 0x4: { // 8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
            int result{ V[opcode >> 8 & 0xF] + V[opcode >> 4 & 0xF] };
            unsigned char flag = result > 255 ? 1 : 0;
            V[opcode >> 8 & 0xF] += V[opcode >> 4 & 0xF];
            V[0xF] = flag;
            break;
        }
        case 0x5: {// 8XY5: VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not
            unsigned char flag = V[opcode >> 8 & 0xF] >= V[opcode >> 4 & 0xF] ? 1 : 0;
            V[opcode >> 8 & 0xF] -= V[opcode >> 4 & 0xF];
            V[0xF] = flag;
            break;
        }
        case 0x6: {// 8XY6: Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
            unsigned char flag = V[opcode >> 4 & 0xF] & 0x1;
            V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] >> 1;
            V[0xF] = flag;
            break;
        }
        case 0x7: {// 8XY7: Sets VX to VY minus VX
            unsigned char flag = V[opcode >> 8 & 0xF] <= V[opcode >> 4 & 0xF] ? 1 : 0;
            V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] - V[opcode >> 8 & 0xF];
            V[0xF] = flag;
            break;
        }
        case 0xE: {// 8XYE: Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset
            unsigned char flag = (V[opcode >> 4 & 0xF] & 0x80) >> 7;
            V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] << 1;
            V[0xF] = flag;
            break;
        }
        }
        break;

    case 0x9: // 9XY0: Skips the next instruction if VX does not equal VY
        if (V[opcode >> 8 & 0xF] != V[opcode >> 4 & 0xF]) { // 9XY0: Skips the next instruction if VX does not equal VY
            PC += 2;
        }
        break;

    case 0xA: // ANNN: Sets I to the address NNN
        I = opcode & 0xFFF; // ANNN: Sets I to the address NNN
        break;

    case 0xB: // BNNN: Jumps to the address NNN plus V0
        PC = V[0x0] + (opcode & 0xFFF); // BNNN: Jumps to the address NNN plus V0
        break;

    case 0xC: {// CXNN: Sets VX to the result of a bitwise and operation on a random number (0 to 255) and NN
        std::random_device rd; // CXNN: Sets VX to the result of a bitwise and operation on a random number (0 to 255) and NN
        std::mt19937 eng(rd());
        std::uniform_int_distribution<> random(0, 255);

        V[opcode >> 8 & 0xF] = static_cast<unsigned char>(random(eng)) & (opcode & 0xFF);
        break;
    }

    case 0xD: {// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Sprite is at address in I
        drawFlag = true; // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Sprite is at address in I
        int n = opcode & 0xF;
        int X = V[opcode >> 8 & 0xF];
        int Y = V[opcode >> 4 & 0xF];

        X = X % constants::RES_W;
        Y = Y % constants::RES_H;

        V[0xF] = 0;
        for (int i = 0; i < n; i++) {
            unsigned char row{ mem[I + i] };
            for (int j = 0; j < 8; j++) {
                if (X + j < constants::RES_W && Y < constants::RES_H) {
                    if (V[0xF] != 1) {
                        // Checks if the screen at the current pos and the sprite pixel are both 1
                        // This causes a collision and VF needs to be set to 1 in that case
                        V[0xF] = ((screen[Y][X + j] == 1) && ((row >> 7 - j & 0x1) == 1)) ? 1 : 0;
                    }
                    screen[Y][X + j] ^= row >> 7 - j & 0x1;
                }
            }
            Y++;
        }

        break;
    }
    case 0xE:
        switch (opcode & 0xFF) { 
        case 0x9E: // EX9E: Skips the next instruction if the key stored in VX is pressed
            if (key[V[opcode >> 8 & 0xF]] == 1) {
                PC += 2;
            }
            break;
        case 0xA1: // EXA1: Skips the next instruction if the key stored in VX is not pressed 
            if (key[V[opcode >> 8 & 0xF]] == 0) {
                PC += 2;
            }
            break;
        }
        break;

    case 0xF:
        switch (opcode & 0xFF) {
        // TODO: FX29
        case 0x07: // FX07: Sets VX to the value of the delay timer
            V[opcode >> 8 & 0xF] = delayTimer;
            break;
        case 0x0A: { // FX0A: A key press is awaited, and then stored in VX. Halts all operation
            bool keyPressed{ false };
            for (int i = 0; i < constants::NUM_KEYS; i++) { // Prioritizes lower key values over higher ones if multiple are pressed
                if (key[i] == 1) {
                    V[opcode >> 8 & 0xF] = i;
                    keyPressed = true;
                }
            }

            if (!keyPressed) {
                PC -= 2;
            }
            break;
        }
        case 0x15: // FX15: Sets the delay timer to VX
            delayTimer = V[opcode >> 8 & 0xF];
            break;
        case 0x18: // FX18: Sets the sound timer to VX
            soundTimer = V[opcode >> 8 & 0xF];
            break;
        case 0x1E: // FX1E: Adds VX to I. VF is not affected
            I += V[opcode >> 8 & 0xF];
            break;
        case 0x29: // FX29: Sets I to the location of the sprite for the character in VX (lowst nibble)
            I = (V[opcode >> 8 & 0xF] & 0xFF) * 5;
            break;
        case 0x33: { // FX33: Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
            int num = V[opcode >> 8 & 0xF];
            for (int i = 0; i < 3; i++) {
                mem[I + i] = static_cast<int>(num / std::pow(10.0, 2 - i)) % 10;
            }
            break;
        }
        case 0x55: // FX55: Stores from V0 to VX (including VX) in memory, starting at address I
            for (int i = 0; i <= (opcode >> 8 & 0xF); i++) {
                mem[I] = V[i];
                I++;
            }
            break;
        case 0x65: // FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I
            for (int i = 0; i <= (opcode >> 8 & 0xF); i++) {
                V[i] = mem[I];
                I++;
            }
            break;
        }
        break;

    default:
        std::cout << "Unable to identify opcode at address " << PC - 2 << std::endl;
        break;
    }

    // Timers
    long long int currTime{ getTime() };
    long long int delta{ currTime - prevTime };
    if (delta > 100) { // Hardcap on the delta so that it does not get too big
        delta = 100;
    }
    prevTime = currTime;
    timerAccumlator += delta;

    while (timerAccumlator > 16.66) { // 16.66 ms is roughly 1/60th of a second
        // Decrement timers if they're above 0
        soundTimer -= soundTimer > 0 ? 1 : 0;
        delayTimer -= delayTimer > 0 ? 1 : 0;

        timerAccumlator -= 16.66; // Subtract 1/60th of a second. Not set to 0 as we may lose time that way
    }
}

long long int chip8::getTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void chip8::x0(unsigned short opcode) {
    switch (opcode) {
    case 0x00E0: // 00E0: Clears the screen
        for (int i = 0; i < constants::RES_H; i++) {
            for (int j = 0; j < constants::RES_W; j++) {
                screen[i][j] = 0;
            }
        }
        break;

    case 0x00EE: // 00EE: Returns from subroutine
        sp--;
        PC = stack[sp];
        break;
    }
}

void chip8::x1(unsigned short opcode) {
    PC = opcode & 0xFFF; // 1NNN: Jumps to address NNN
}

void chip8::x2(unsigned short opcode) {
    stack[sp] = PC; // 2NNN: Calls subroutine at NNN
    sp++;
    PC = opcode & 0xFFF;
}

void chip8::x3(unsigned short opcode) {
    if (V[opcode >> 8 & 0xF] == (opcode & 0xFF)) { // 3XNN: Skips the next instruction if VX equals NN 
        PC += 2;
    }
}

void chip8::x4(unsigned short opcode) {
    if (V[opcode >> 8 & 0xF] != (opcode & 0xFF)) { // 4XNN: Skips the next instruction if VX does not equal NN
        PC += 2;
    }
}

void chip8::x5(unsigned short opcode) {
    if (V[opcode >> 8 & 0xF] == V[opcode >> 4 & 0xF]) { // 5XY0: Skips the next instruction if VX equals VY
        PC += 2;
    }
}

void chip8::x6(unsigned short opcode) {
    V[opcode >> 8 & 0xF] = opcode & 0xFF; // 6XNN: Sets VX to NN
}

void chip8::x7(unsigned short opcode) {
    V[opcode >> 8 & 0xF] += opcode & 0xFF; // 7XNN: Adds NN to VX (carry flag is not changed)
}

void chip8::x8(unsigned short opcode) {
    switch (opcode & 0xF) {
    case 0x0: // 8XY0: Sets VX to the value of VY
        V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF];
        break;
    case 0x1: // 8XY1: Sets VX to VX or VY
        V[opcode >> 8 & 0xF] |= V[opcode >> 4 & 0xF];
        V[0xF] = 0;
        break;
    case 0x2: // 8XY2: Sets VX to VX and VY
        V[opcode >> 8 & 0xF] &= V[opcode >> 4 & 0xF];
        V[0xF] = 0;
        break;
    case 0x3: // 8XY3: Sets VX to VX xor VY
        V[opcode >> 8 & 0xF] ^= V[opcode >> 4 & 0xF];
        V[0xF] = 0;
        break;
    case 0x4: { // 8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
        int result{ V[opcode >> 8 & 0xF] + V[opcode >> 4 & 0xF] };
        unsigned char flag = result > 255 ? 1 : 0;
        V[opcode >> 8 & 0xF] += V[opcode >> 4 & 0xF];
        V[0xF] = flag;
        break;
    }
    case 0x5: {// 8XY5: VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not
        unsigned char flag = V[opcode >> 8 & 0xF] >= V[opcode >> 4 & 0xF] ? 1 : 0;
        V[opcode >> 8 & 0xF] -= V[opcode >> 4 & 0xF];
        V[0xF] = flag;
        break;
    }
    case 0x6: {// 8XY6: Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
        unsigned char flag = V[opcode >> 4 & 0xF] & 0x1;
        V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] >> 1;
        V[0xF] = flag;
        break;
    }
    case 0x7: {// 8XY7: Sets VX to VY minus VX
        unsigned char flag = V[opcode >> 8 & 0xF] <= V[opcode >> 4 & 0xF] ? 1 : 0;
        V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] - V[opcode >> 8 & 0xF];
        V[0xF] = flag;
        break;
    }
    case 0xE: {// 8XYE: Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset
        unsigned char flag = (V[opcode >> 4 & 0xF] & 0x80) >> 7;
        V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] << 1;
        V[0xF] = flag;
        break;
    }
    }
}

void chip8::x9(unsigned short opcode) {
    if (V[opcode >> 8 & 0xF] != V[opcode >> 4 & 0xF]) { // 9XY0: Skips the next instruction if VX does not equal VY
        PC += 2;
    }
}

void chip8::xA(unsigned short opcode) {
    I = opcode & 0xFFF; // ANNN: Sets I to the address NNN
}

void chip8::xB(unsigned short opcode) {
    PC = V[0x0] + (opcode & 0xFFF); // BNNN: Jumps to the address NNN plus V0
}

void chip8::xC(unsigned short opcode) {
    std::random_device rd; // CXNN: Sets VX to the result of a bitwise and operation on a random number (0 to 255) and NN
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> random(0, 255);

    V[opcode >> 8 & 0xF] = static_cast<unsigned char>(random(eng)) & (opcode & 0xFF);
}

void chip8::xD(unsigned short opcode) {
    drawFlag = true; // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Sprite is at address in I
    int n = opcode & 0xF;
    int X = V[opcode >> 8 & 0xF];
    int Y = V[opcode >> 4 & 0xF];

    X = X % constants::RES_W;
    Y = Y % constants::RES_H;

    V[0xF] = 0;
    for (int i = 0; i < n; i++) {
        unsigned char row{ mem[I + i] };
        for (int j = 0; j < 8; j++) {
            if (X + j < constants::RES_W && Y < constants::RES_H) {
                if (V[0xF] != 1) {
                    // Checks if the screen at the current pos and the sprite pixel are both 1
                    // This causes a collision and VF needs to be set to 1 in that case
                    V[0xF] = ((screen[Y][X + j] == 1) && ((row >> 7 - j & 0x1) == 1)) ? 1 : 0;
                }
                screen[Y][X + j] ^= row >> 7 - j & 0x1;
            }
        }
        Y++;
    }
}

void chip8::xE(unsigned short opcode) {
    switch (opcode & 0xFF) {
    case 0x9E: // EX9E: Skips the next instruction if the key stored in VX is pressed
        if (key[V[opcode >> 8 & 0xF]] == 1) {
            PC += 2;
        }
        break;
    case 0xA1: // EXA1: Skips the next instruction if the key stored in VX is not pressed 
        if (key[V[opcode >> 8 & 0xF]] == 0) {
            PC += 2;
        }
        break;
    }
}

void chip8::xF(unsigned short opcode) {
    switch (opcode & 0xFF) {
        // TODO: FX29
    case 0x07: // FX07: Sets VX to the value of the delay timer
        V[opcode >> 8 & 0xF] = delayTimer;
        break;
    case 0x0A: { // FX0A: A key press is awaited, and then stored in VX. Halts all operation
        bool keyPressed{ false };
        for (int i = 0; i < constants::NUM_KEYS; i++) { // Prioritizes lower key values over higher ones if multiple are pressed
            if (key[i] == 1) {
                V[opcode >> 8 & 0xF] = i;
                keyPressed = true;
            }
        }

        if (!keyPressed) {
            PC -= 2;
        }
        break;
    }
    case 0x15: // FX15: Sets the delay timer to VX
        delayTimer = V[opcode >> 8 & 0xF];
        break;
    case 0x18: // FX18: Sets the sound timer to VX
        soundTimer = V[opcode >> 8 & 0xF];
        break;
    case 0x1E: // FX1E: Adds VX to I. VF is not affected
        I += V[opcode >> 8 & 0xF];
        break;
    case 0x29: // FX29: Sets I to the location of the sprite for the character in VX (lowst nibble)
        I = (V[opcode >> 8 & 0xF] & 0xFF) * 5;
        break;
    case 0x33: { // FX33: Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
        int num = V[opcode >> 8 & 0xF];
        for (int i = 0; i < 3; i++) {
            mem[I + i] = static_cast<int>(num / std::pow(10.0, 2 - i)) % 10;
        }
        break;
    }
    case 0x55: // FX55: Stores from V0 to VX (including VX) in memory, starting at address I
        for (int i = 0; i <= (opcode >> 8 & 0xF); i++) {
            mem[I] = V[i];
            I++;
        }
        break;
    case 0x65: // FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I
        for (int i = 0; i <= (opcode >> 8 & 0xF); i++) {
            V[i] = mem[I];
            I++;
        }
        break;
    }
}