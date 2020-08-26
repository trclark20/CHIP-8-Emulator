#include <SDL.h>
#include <SDL_ttf.h>
#include "chip8.h"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "display.h"

int main(int argc, char **argv)
{	
	display _display;
	chip8 _chip8;
	int counter = 0;
	bool quit = false;

    _chip8.initialize();

    const char* path = argv[1];
    _chip8.loadGame(path);

    while (!quit)
    {
		if (_display.newFile)
		{
			_chip8.initialize();
			_chip8.loadGame(_display.dropped_filedir);
			_display.newFile = false;
		}

		counter++;
		_display.calculateFps();

		std::this_thread::sleep_for(std::chrono::milliseconds(3/2));
		
		_chip8.emulateCycle(1);

		if (counter == 10)
		{
			if (_display.showDebug)
				_display.drawDebug(_chip8.totalInstructions, _chip8.pc);
			_chip8.updateTimers();
			counter = 0;
		}

		if (_chip8.drawFlag)
		{
			_chip8.drawFlag = _display.draw(_chip8.gfx, _chip8.drawFlag);
		}

		quit = _display.processInput(_chip8.key, quit);
    }

	return 0;
}