#pragma once
#include <memory>
#include "SDL.h"
#include "SDL_mixer.h"

// Deleter to cleanup SDL objects
struct SDLDeleter {
	void operator()(SDL_Window* window) const { if (window) SDL_DestroyWindow(window); }
	void operator()(SDL_Renderer* renderer) const { if (renderer) SDL_DestroyRenderer(renderer); }
	void operator()(Mix_Chunk* chunk) const { if (chunk) Mix_FreeChunk(chunk); }
};

class interface {
public:
	interface();
	~interface();

	bool handleKeys(unsigned char keys[16]);
	void updateScreen(unsigned char screen[32][64]);
	void playBeep();

	static const int SCREEN_WIDTH = 1280;
	static const int SCREEN_HEIGHT = 640;

private:
	std::unique_ptr<SDL_Window, SDLDeleter> window;
	std::unique_ptr<SDL_Renderer, SDLDeleter> renderer;
	std::unique_ptr<Mix_Chunk, SDLDeleter> beep;
};
