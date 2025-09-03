#include <iostream>
#include <chrono>
#include "chip8.h"
#include "interface.h"
#include "constants.h"

long long int getTime();

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
	float accumlator{ 0 };
	long long int prevTime{ getTime() };
	while (isRunning) {
		isRunning = interface.handleKeys(chip.key);
		long long int currTime{ getTime() };
		long long int delta{ currTime - prevTime };
		prevTime = currTime;
		accumlator += delta;
		while (accumlator > 16.66) {
			for (int i = 0; i < constants::CYCLES_PER_FRAME; i++) {
				chip.runCycle();
			}

			if (chip.drawFlag) {
				interface.updateScreen(chip.screen);
				chip.drawFlag = false;
			}

			if (chip.soundTimer > 0) {
				interface.playBeep();
			}

			accumlator -= 16.66;
		}
	}
	
	return 0;
}

long long int getTime() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}