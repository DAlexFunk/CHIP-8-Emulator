#include <iostream>
#include "interface.h"

interface::interface() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "Could not initilize: " << SDL_GetError() << std::endl;
	}

	window.reset(SDL_CreateWindow(
		"CHIP-8",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN));

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
bool interface::handleKeys(unsigned char keys[constants::NUM_KEYS]) {
	SDL_Event evt;
	while (SDL_PollEvent(&evt) > 0) {
		if (evt.type == SDL_QUIT) {
			return false;
		}
		else if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
			int state = evt.type == SDL_KEYDOWN ? 1 : 0;
			
			/*
			KEY MAPPING:
			1 2 3 4			 1 2 3 C
			Q W E R			 4 5 6 D
			A S D F    ->    7 8 9 E
			Z X C V			 A 0 B F
			*/
			switch (evt.key.keysym.sym) {
			case SDLK_1:
				keys[0x1] = state;
				break;
			case SDLK_2:
				keys[0x2] = state;
				break;
			case SDLK_3:
				keys[0x3] = state;
				break;
			case SDLK_4:
				keys[0xC] = state;
				break;
			case SDLK_q:
				keys[0x4] = state;
				break;
			case SDLK_w:
				keys[0x5] = state;
				break;
			case SDLK_e:
				keys[0x6] = state;
				break;
			case SDLK_r:
				keys[0xD] = state;
				break;
			case SDLK_a:
				keys[0x7] = state;
				break;
			case SDLK_s:
				keys[0x8] = state;
				break;
			case SDLK_d:
				keys[0x9] = state;
				break;
			case SDLK_f:
				keys[0xE] = state;
				break;
			case SDLK_z:
				keys[0xA] = state;
				break;
			case SDLK_x:
				keys[0x0] = state;
				break;
			case SDLK_c:
				keys[0xB] = state;
				break;
			case SDLK_v:
				keys[0xF] = state;
				break;
			default:
				break;
			}
		}
	}

	return true;
}

void interface::updateScreen(unsigned char screen[constants::RES_H][constants::RES_W]) {
	// Nothing for now
}

/*
 * Function playBeep()
 * Takes no parameters
 * Returns no value
 * Plays a beeping sound
 */
void interface::playBeep() {
	Mix_PlayChannel(-1, beep.get(), 0);
}