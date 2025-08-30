#pragma once
#include <string>
#include "constants.h"

class chip8 {
public:
	chip8();
	~chip8();

	bool drawFlag;
	bool soundFlag;

	bool loadFile(std::string file);
	void runCycle();

	unsigned char key[constants::NUM_KEYS]{ 0 };	   // Stores the current key state
	unsigned char screen[constants::RES_H][constants::RES_W]{ 0 }; // The screen where a 1 is white and a 0 is black
private:
	unsigned short PC{ 0 };		   // Program Counter
	unsigned short I{ 0 };		   // Address Register
	unsigned short opcode{ 0 };	   // Current Opcode

	unsigned char V[16]{ 0 };	   // Registers: 16 8-bit registers named V0 - VF
	unsigned char mem[constants::MEM_LOCATIONS]{ 0 };  // Memory: 4096 8-bit memory locations
	
	unsigned short stack[16]{ 0 }; // Stack for subroutine calls
	unsigned short sp{ 0 };		   // Stack pointer

	unsigned char delayTimer{ 0 }; // Delay timer used for game events
	unsigned char soundTimer{ 0 }; // Sound timer that plays a sound at 0
};