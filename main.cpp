#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

bool init();
bool loadSoundEffect(std::string path);
void close();

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Mix_Chunk* beep = nullptr;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 640;

int main(int argc, char* argv[]) {
	if (!init()) {
		return 1;
	}

	if (!loadSoundEffect("beep.wav")) {
		return 1;
	}

	bool isRunning = true;
	SDL_Event evt;
	while (isRunning) {
		while (SDL_PollEvent(&evt) > 0) {
			if (evt.type == SDL_QUIT) {
				isRunning = false;
			}
		}
	}
	
	close();
	return 0;
}

bool init() {
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

void close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_FreeChunk(beep);

	window = nullptr;
	renderer = nullptr;
	beep = nullptr;

	SDL_Quit();
}