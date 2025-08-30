#include <iostream>
#include "chip8.h"
#include "interface.h"

int main(int argc, char* argv[]) {
	chip8 chip;
	interface interface;

	if (!chip.loadFile("tests/test1.ch8")) {
		return 1;
	}

	bool isRunning = true;
	while (isRunning) {
		isRunning = interface.handleKeys(chip.key);
	}
	
	return 0;
}