#include <SDL.h>

class display
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Surface* messageSurface;
	SDL_Texture* messageTexture;
	SDL_Color WHITE = { 255, 255, 255 };
	TTF_Font* Sans;

	Uint32 startTime;

	bool redrawWindow = false;

	int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 640;
	const int GAME_WIDTH = 1280;
	const int GAME_HEIGHT = 640;
	const int SCALE_X = GAME_WIDTH / 64;
	const int SCALE_Y = GAME_HEIGHT / 32;

public:
	int frameNumber = 0;
	int cpsCalcNumber = 0;
	int fpsCalcNumber = 0;

	bool showDebug = false;
	bool newFile = false;
	char* dropped_filedir;

	display();
	~display();
	void calculateFps();
	bool draw(unsigned char(&gfx)[64 * 32], bool drawFlag);
	void drawDebug(int totalInstructions, unsigned short pc);
	bool processInput(uint8_t keys[16], bool quit);
};