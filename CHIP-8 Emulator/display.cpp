#include <SDL_ttf.h>
#include "display.h"
#include "chip8.h"
#include <sstream>
#include <iomanip>

display::display()
{
	TTF_Init();
	startTime = SDL_GetTicks();
	Sans = TTF_OpenFont("..\\Fonts\\arial.ttf", 30);

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

display::~display()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void display::calculateFps()
{
	cpsCalcNumber++;
	if (startTime < SDL_GetTicks() - (1.0 * 1000))
	{
		startTime = SDL_GetTicks();

		std::ostringstream s;
		s << "CPS: " << cpsCalcNumber << " FPS: " << fpsCalcNumber;
		std::string str = s.str();
		SDL_SetWindowTitle(window, str.c_str());
		cpsCalcNumber = 0;
		fpsCalcNumber = 0;
	}
}

bool display::draw(unsigned char (&gfx)[64 * 32], bool drawFlag)
{
	if (redrawWindow)
	{
		SDL_SetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
		redrawWindow = false;
	}

	fpsCalcNumber++;
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	int rownum = 0;
	SDL_Rect pixel;

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			pixel.x = x * SCALE_X;
			pixel.y = y * SCALE_Y;
			pixel.w = SCALE_X;
			pixel.h = SCALE_Y;

			rownum = y * 64;
			if (gfx[x + rownum] == 1)
				SDL_RenderFillRect(renderer, &pixel);
		}
	}

	frameNumber++;
	drawFlag = false;

	if (!showDebug)
		SDL_RenderPresent(renderer);

	return drawFlag;
}

void display::drawDebug(int totalInstructions, unsigned short pc, unsigned char memory[4096])
{
	SDL_Rect pixel;

	SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);

	pixel.x = 64 * SCALE_X;
	pixel.y = 0;
	pixel.w = WINDOW_WIDTH - GAME_WIDTH;
	pixel.h = WINDOW_HEIGHT;

	SDL_RenderFillRect(renderer, &pixel);

	std::ostringstream s;
	s << "FRAME: " << frameNumber << "\n" << "CLOCKS: " << totalInstructions <<
		"\n" << "PC: $" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << std::hex << pc <<
		"\n"  << "$" << std::setfill('0') << std::setw(4) << std::right << std::uppercase << std::hex << pc << ": " << memory[0];
	std::string str = s.str();

	messageSurface = TTF_RenderText_Blended_Wrapped(Sans, str.c_str(), WHITE, WINDOW_WIDTH - GAME_WIDTH);
	messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);

	SDL_Rect Message_rect;
	Message_rect.x = 64 * SCALE_X;
	Message_rect.y = 0;
	Message_rect.w = WINDOW_WIDTH - GAME_WIDTH;
	Message_rect.h = 100;

	SDL_RenderCopy(renderer, messageTexture, NULL, &Message_rect);

	SDL_RenderPresent(renderer);

	SDL_FreeSurface(messageSurface);
	SDL_DestroyTexture(messageTexture);
}

bool display::processInput(uint8_t keys[16], bool quit)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}

		if (event.type == SDL_DROPFILE)
		{
			dropped_filedir = event.drop.file;
			newFile = true;
			SDL_free(dropped_filedir);
			break;
		}

		if (event.type == SDL_KEYDOWN)
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

			case SDLK_p:
			{
				showDebug = !showDebug;
				redrawWindow = true;
				if (showDebug)
					WINDOW_WIDTH = 1920;
				else
					WINDOW_WIDTH = 1280;
			} break;

			case SDLK_o:
			{
				speedup = true;
			} break;
			case SDLK_LEFTBRACKET:
			{
				decreaseSpeed = true;
			} break;
			case SDLK_RIGHTBRACKET:
			{
				increaseSpeed = true;
			} break;
			}
		}

		if (event.type == SDL_KEYUP)
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

			case SDLK_o:
			{
				speedup = false;
			} break;
			}
		} break;
		}
		return quit;
}