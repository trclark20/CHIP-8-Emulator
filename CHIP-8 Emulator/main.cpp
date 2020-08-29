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
#include "sound.h"
#include <Windows.h>

void reset(chip8 &_chip8, display &_display);

int main(int argc, char **argv)
{	
	display _display;
	chip8 _chip8;
	sound _sound;

	double frequency = 500.0 / 1.0;
	int SLEEP_TIME = 1;
	int counter = 0;
	bool quit = false;

    _chip8.initialize();

    const char* path = argv[1];
    _chip8.loadGame(path);

    while (!quit)
    {
		_chip8.emulateCycle(1);
		_display.calculateFps();

		if (_display.newFile)
		{
			reset(_chip8, _display);
		}

		counter++;

		if (_display.speedup)
			SLEEP_TIME = 0;
		else
			SLEEP_TIME = 1;

		if (counter == 10)
		{
			if (_display.showDebug)
				_display.drawDebug(_chip8.totalInstructions, _chip8.pc, _chip8.memory);
			_chip8.updateTimers();
			counter = 0;
		}

		if (_chip8.playSound)
		{
			_chip8.playSound = false;
			_sound.playSound();
		}

		if (_display.increaseSpeed)
		{
			_display.increaseSpeed = false;
		}

		if (_display.decreaseSpeed)
		{
			_display.decreaseSpeed = false;
		}

		if (_chip8.drawFlag)
			_chip8.drawFlag = _display.draw(_chip8.gfx, _chip8.drawFlag);

		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));

		quit = _display.processInput(_chip8.key, quit);
    }

	return 0;
}

void reset(chip8 &_chip8, display &_display)
{
	int counter = 0;
	bool quit = false;

	_chip8.initialize();

	_chip8.loadGame(_display.dropped_filedir);
	_display.newFile = false;
}