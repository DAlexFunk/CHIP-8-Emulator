#include <iostream>
#include "chip8.h"
#include "interface.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Must pass in the rom as an argument\n";
		return 1;
	}
	
	chip8 chip;
	interface interface;

	if (!chip.loadFile(argv[1])) {
		return 1;
	}

	bool isRunning = true;
	while (isRunning) {
		isRunning = interface.handleKeys(chip.key);
	}
	
	return 0;
}