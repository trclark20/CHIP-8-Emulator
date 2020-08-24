#include <SDL.h>
#include "chip8.h"

chip8 _chip8;

int windowWidth = 640;
int windowHeight = 320;
int sx = windowWidth / 64;
int sy = windowHeight / 32;

bool ready = false;
bool quit = false;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

int setupGraphics();
void update();
void drawGraphics();
bool ProcessInput(uint8_t* keys);

int main(int argc, char **argv)
{
    setupGraphics();

    _chip8.initialize();

    const char* path = "..\\chip8 games\\PONG";
    _chip8.loadGame(path);

    while (!quit)
    {
		update();

		drawGraphics();
		quit = ProcessInput(_chip8.key);
    }

	return 0;
}

int setupGraphics()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8 Emulator", 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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
			pixel.x = x * sx;
			pixel.y = y * sy;
			pixel.w = 10;
			pixel.h = 10;
			rownum = y * 64;
			if (_chip8.gfx[x + rownum] == 1)
				SDL_RenderFillRect(renderer, &pixel);
		}
	}
	SDL_RenderPresent(renderer);
	_chip8.drawFlag = false;
}

void update() {
	_chip8.emulateCycle(10);
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