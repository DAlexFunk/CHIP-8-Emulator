#pragma once
#include <memory>
#include "SDL.h"
#include "SDL_mixer.h"
#include "constants.h"

// Deleter to cleanup SDL objects
struct SDLDeleter {
	void operator()(SDL_Window* window) const { if (window) SDL_DestroyWindow(window); }
	void operator()(SDL_Renderer* renderer) const { if (renderer) SDL_DestroyRenderer(renderer); }
	void operator()(Mix_Chunk* chunk) const { if (chunk) Mix_FreeChunk(chunk); }
};

/*
KEY MAPPING:
1 2 3 4		 	 1 2 3 C
Q W E R		 	 4 5 6 D
A S D F    ->    7 8 9 E
Z X C V			 A 0 B F
*/
class interface {
public:
	interface();
	~interface();

	bool handleKeys(unsigned char keys[constants::NUM_KEYS]);
	void updateScreen(unsigned char screen[constants::RES_H][constants::RES_W]);
	void playBeep();

private:
	std::unique_ptr<SDL_Window, SDLDeleter> window;
	std::unique_ptr<SDL_Renderer, SDLDeleter> renderer;
	std::unique_ptr<Mix_Chunk, SDLDeleter> beep;
};
