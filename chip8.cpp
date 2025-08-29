#include <fstream>
#include <iostream>
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

chip8::chip8() {
	// Nothing
}

chip8::~chip8() {
	// Nothing
}

/*
 * Function init()
 * Takes no parameters
 * Returns no value
 * Sets all member vars to default states
 */
void chip8::initChip() {
    // Reset Vars
    PC = 0x200;
    I = 0;
    opcode = 0;
    sp = 0;
    drawFlag = false;
    soundFlag = false;
    delayTimer = 0;
    soundTimer = 0;

    // Reset registers, stack, and keys
    for (int i = 0; i < 16; i++) {
        V[i] = 0;
        stack[i] = 0;
        key[i] = 0;
    }

    // Reset memory
    for (int i = 0; i < 4096; i++) {
        mem[i] = 0;
    }

    // Reset Screen
    for (int i = 0; i < RES_H; i++) {
        for (int j = 0; j < RES_W; j++) {
            screen[i][j] = 0;
        }
    }

    // Load font into memory
    for (int i = 0; i < 80; i++) {
        mem[i] = font[i];
    }
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

    int i{ 512 };
    unsigned char curr{ 0 };
    while (file >> curr) {
        if (i > 4095) {
            std::cout << path << " is too large\n";
            return false;
        }

        mem[i] = curr;
        i++;
    }
    return true;
}

/*
 * Function runCycle()
 * Takes no parameters
 * Returns no value
 * Simulates one chip cycle: Fetch -> Decode -> Execute -> Update Timers
 */
void chip8::runCycle() {

}