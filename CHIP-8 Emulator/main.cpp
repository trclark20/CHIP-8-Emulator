#include <SDL.h>
#include <SDL_ttf.h>
#include "chip8.h"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

chip8 _chip8;

int frameNumber = 0;

int windowWidth = 1920;
int windowHeight = 640;
int renderWidth = 1280;
int renderHeight = 640;
int scaleX = renderWidth / 64;
int scaleY = renderHeight / 32;

int counter = 0;

bool ready = false;
bool quit = false;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
TTF_Font* Sans;
SDL_Surface* surfaceMsg;
SDL_Texture* Message;
SDL_Color White = { 255, 255, 255 };

int setupGraphics();
void update();
void drawGraphics();
void drawDebug();
bool ProcessInput(uint8_t* keys);

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
int actualFrames = 0;

int main(int argc, char **argv)
{
	double fps = 0;
	Uint32 startTime = SDL_GetTicks();
    setupGraphics();

    _chip8.initialize();

    const char* path = "..\\chip8 games\\PONG2";
    _chip8.loadGame(path);

    while (!quit)
    {
		actualFrames++;
		Uint32 elapsedMS = SDL_GetTicks() - startTime;
		if (elapsedMS) {
			double elapsedSeconds = elapsedMS / 1000.0;
			fps = actualFrames / elapsedSeconds;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(3/2));
		std::cout << std::string(50, '\n');
		printf("Fps: %f", fps);
		printf("Frame: %d", frameNumber);
		printf("Total instructions: %d", _chip8.totalInstructions);
		update();
		counter++;

		if (counter == 10)
		{
			drawDebug();
			_chip8.updateTimers();
			counter = 0;
		}

		if (_chip8.drawFlag)
		{
			drawGraphics();
			frameNumber++;
		}

		quit = ProcessInput(_chip8.key);
    }

	return 0;
}

int setupGraphics()
{
	TTF_Init();
	Sans = TTF_OpenFont("..\\Fonts\\arial.ttf", 30);

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    return 0;
}

void drawGraphics()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	int rownum = 0;
	SDL_Rect pixel;

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			pixel.x = x * scaleX;
			pixel.y = y * scaleY;
			pixel.w = scaleX;
			pixel.h = scaleY;

			rownum = y * 64;
			if (_chip8.gfx[x + rownum] == 1)
				SDL_RenderFillRect(renderer, &pixel);
		}
	}
	_chip8.drawFlag = false;
}

void drawDebug()
{
	SDL_Rect pixel;

	SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);

	pixel.x = 64 * scaleX;
	pixel.y = 0;
	pixel.w = windowWidth - renderWidth;
	pixel.h = windowHeight;

	SDL_RenderFillRect(renderer, &pixel);

	std::ostringstream s;
	s << "ACTUAL FRAME: " << actualFrames << "\n" << "FRAME: " << frameNumber << "\n" << "CLOCKS: " << _chip8.totalInstructions <<
		"\n" << "PC: " << std::setfill('0') << std::setw(4) << std::right << std::uppercase << std::hex << _chip8.pc;
	std::string str = s.str();

	surfaceMsg = TTF_RenderText_Blended_Wrapped(Sans, str.c_str(), White, windowWidth - renderWidth);
	Message = SDL_CreateTextureFromSurface(renderer, surfaceMsg);

	SDL_Rect Message_rect;
	Message_rect.x = 64 * scaleX;
	Message_rect.y = 0;
	Message_rect.w = windowWidth - renderWidth;
	Message_rect.h = 100;

	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	SDL_RenderPresent(renderer);

	SDL_FreeSurface(surfaceMsg);
	SDL_DestroyTexture(Message);
}

void update() {
	_chip8.emulateCycle(1);
}

bool ProcessInput(uint8_t* keys)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
		{
			quit = true;
		} break;

		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			{
				quit = true;
			} break;

			case SDLK_x:
			{
				keys[0] = 1;
			} break;

			case SDLK_1:
			{
				keys[1] = 1;
			} break;

			case SDLK_2:
			{
				keys[2] = 1;
			} break;

			case SDLK_3:
			{
				keys[3] = 1;
			} break;

			case SDLK_q:
			{
				keys[4] = 1;
			} break;

			case SDLK_w:
			{
				keys[5] = 1;
			} break;

			case SDLK_e:
			{
				keys[6] = 1;
			} break;

			case SDLK_a:
			{
				keys[7] = 1;
			} break;

			case SDLK_s:
			{
				keys[8] = 1;
			} break;

			case SDLK_d:
			{
				keys[9] = 1;
			} break;

			case SDLK_z:
			{
				keys[0xA] = 1;
			} break;

			case SDLK_c:
			{
				keys[0xB] = 1;
			} break;

			case SDLK_4:
			{
				keys[0xC] = 1;
			} break;

			case SDLK_r:
			{
				keys[0xD] = 1;
			} break;

			case SDLK_f:
			{
				keys[0xE] = 1;
			} break;

			case SDLK_v:
			{
				keys[0xF] = 1;
			} break;
			}
		} break;

		case SDL_KEYUP:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_x:
			{
				keys[0] = 0;
			} break;

			case SDLK_1:
			{
				keys[1] = 0;
			} break;

			case SDLK_2:
			{
				keys[2] = 0;
			} break;

			case SDLK_3:
			{
				keys[3] = 0;
			} break;

			case SDLK_q:
			{
				keys[4] = 0;
			} break;

			case SDLK_w:
			{
				keys[5] = 0;
			} break;

			case SDLK_e:
			{
				keys[6] = 0;
			} break;

			case SDLK_a:
			{
				keys[7] = 0;
			} break;

			case SDLK_s:
			{
				keys[8] = 0;
			} break;

			case SDLK_d:
			{
				keys[9] = 0;
			} break;

			case SDLK_z:
			{
				keys[0xA] = 0;
			} break;

			case SDLK_c:
			{
				keys[0xB] = 0;
			} break;

			case SDLK_4:
			{
				keys[0xC] = 0;
			} break;

			case SDLK_r:
			{
				keys[0xD] = 0;
			} break;

			case SDLK_f:
			{
				keys[0xE] = 0;
			} break;

			case SDLK_v:
			{
				keys[0xF] = 0;
			} break;
			}
		} break;
		}
	}

	return quit;
}