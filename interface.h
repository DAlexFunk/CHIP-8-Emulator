#pragma once
#include <SDL.h>
#include <SDL_mixer.h>

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
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	Mix_Chunk* beep = nullptr;
};
