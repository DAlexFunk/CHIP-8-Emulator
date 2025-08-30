#include <iostream>
#include "interface.h"

interface::interface() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "Could not initilize: " << SDL_GetError() << std::endl;
	}

	window.reset(SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN));
	if (window == NULL) {
		std::cout << "Could not make window: " << SDL_GetError() << std::endl;
	}

	renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));
	if (renderer == NULL) {
		std::cout << "Could not make renderer: " << SDL_GetError() << std::endl;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << "Could not initilize SDL_mixer: " << Mix_GetError() << std::endl;
	}

	beep.reset(Mix_LoadWAV("beep.wav"));
	if (beep == NULL) {
		std::cout << "Could not load beep: " << Mix_GetError() << std::endl;
	}
}

interface::~interface() {
	Mix_Quit();
	SDL_Quit();
}

/*
 * Function handleKeys()
 * Takes the keys array that needs to be changed
 * Returns false if the quit event was called and true otherwise
 * Updates the given array with the state of the keyboard
 */
bool interface::handleKeys(unsigned char keys[16]) {
	SDL_Event evt;
	while (SDL_PollEvent(&evt) > 0) {
		if (evt.type == SDL_QUIT) {
			return false;
		}
		else if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
			int state = evt.type == SDL_KEYDOWN ? 1 : 0;
			
			switch (evt.key.keysym.sym) {
			case SDLK_1:
				keys[1] = state;
				break;
			case SDLK_2:
				keys[2] = state;
				break;
			case SDLK_3:
				keys[3] = state;
				break;
			case SDLK_4:
				keys[12] = state;
				break;
			case SDLK_q:
				keys[4] = state;
				break;
			case SDLK_w:
				keys[5] = state;
				break;
			case SDLK_e:
				keys[6] = state;
				break;
			case SDLK_r:
				keys[13] = state;
				break;
			case SDLK_a:
				keys[7] = state;
				break;
			case SDLK_s:
				keys[8] = state;
				break;
			case SDLK_d:
				keys[9] = state;
				break;
			case SDLK_f:
				keys[14] = state;
				break;
			case SDLK_z:
				keys[10] = state;
				break;
			case SDLK_x:
				keys[0] = state;
				break;
			case SDLK_c:
				keys[11] = state;
				break;
			case SDLK_v:
				keys[15] = state;
				break;
			default:
				break;
			}
		}
	}

	return true;
}

void interface::updateScreen(unsigned char screen[32][64]) {
	// Nothing for now
}

/*
 * Function playBeep()
 * Takes no parameters
 * Returns no value
 * Plays a beeping sound
 */
void interface::playBeep() {
	std::cout << "BEEPING\n";
	Mix_PlayChannel(-1, beep.get(), 0);
}