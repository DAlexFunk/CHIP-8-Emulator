#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include "chip8.h"

bool initSDL();
bool loadSoundEffect(std::string path);
void handleKeys(SDL_Event evt);
void close();

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Mix_Chunk* beep = nullptr;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 640;

chip8 chip;

int main(int argc, char* argv[]) {
	if (!initSDL()) {
		return 1;
	}

	if (!loadSoundEffect("beep.wav")) {
		return 1;
	}

	chip.initChip();

	bool isRunning = true;
	SDL_Event evt;
	while (isRunning) {
		while (SDL_PollEvent(&evt) > 0) {
			if (evt.type == SDL_QUIT) {
				isRunning = false;
			}
			handleKeys(evt);
		}
	}
	
	close();
	return 0;
}

bool initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "Could not initilize: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cout << "Could not make window: " << SDL_GetError() << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "Could not make renderer: " << SDL_GetError() << std::endl;
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << "Could not initilize SDL_mixer: " << Mix_GetError() << std::endl;
		return false;
	}

	return true;
}

bool loadSoundEffect(std::string path) {
	beep = Mix_LoadWAV(path.c_str());
	if (beep == NULL) {
		std::cout << "Could not load " << path << ": " << Mix_GetError() << std::endl;
		return false;
	}

	return true;
}

void handleKeys(SDL_Event evt) {
	if (evt.type == SDL_KEYDOWN) {
		switch (evt.key.keysym.sym) {
		case SDLK_1:
			chip.key[1] = 1;
			break;
		case SDLK_2:
			chip.key[2] = 1;
			break;
		case SDLK_3:
			chip.key[3] = 1;
			break;
		case SDLK_4:
			chip.key[12] = 1;
			break;
		case SDLK_q:
			chip.key[4] = 1;
			break;
		case SDLK_w:
			chip.key[5] = 1;
			break;
		case SDLK_e:
			chip.key[6] = 1;
			break;
		case SDLK_r:
			chip.key[13] = 1;
			break;
		case SDLK_a:
			chip.key[7] = 1;
			break;
		case SDLK_s:
			chip.key[8] = 1;
			break;
		case SDLK_d:
			chip.key[9] = 1;
			break;
		case SDLK_f:
			chip.key[14] = 1;
			break;
		case SDLK_z:
			chip.key[10] = 1;
			break;
		case SDLK_x:
			chip.key[0] = 1;
			break;
		case SDLK_c:
			chip.key[11] = 1;
			break;
		case SDLK_v:
			chip.key[15] = 1;
			break;
		default:
			break;
		}
	}

	else if (evt.type == SDL_KEYUP) {
		switch (evt.key.keysym.sym) {
		case SDLK_1:
			chip.key[1] = 0;
			break;
		case SDLK_2:
			chip.key[2] = 0;
			break;
		case SDLK_3:
			chip.key[3] = 0;
			break;
		case SDLK_4:
			chip.key[12] = 0;
			break;
		case SDLK_q:
			chip.key[4] = 0;
			break;
		case SDLK_w:
			chip.key[5] = 0;
			break;
		case SDLK_e:
			chip.key[6] = 0;
			break;
		case SDLK_r:
			chip.key[13] = 0;
			break;
		case SDLK_a:
			chip.key[7] = 0;
			break;
		case SDLK_s:
			chip.key[8] = 0;
			break;
		case SDLK_d:
			chip.key[9] = 0;
			break;
		case SDLK_f:
			chip.key[14] = 0;
			break;
		case SDLK_z:
			chip.key[10] = 0;
			break;
		case SDLK_x:
			chip.key[0] = 0;
			break;
		case SDLK_c:
			chip.key[11] = 0;
			break;
		case SDLK_v:
			chip.key[15] = 0;
			break;
		default:
			break;
		}
	}
}

void close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_FreeChunk(beep);

	window = nullptr;
	renderer = nullptr;
	beep = nullptr;

	SDL_Quit();
}