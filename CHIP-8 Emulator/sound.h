#include <SDL_mixer.h>
#include <string>

class sound
{
private:
	const char* WAV_PATH = "..\\sound\\beep.wav";

	Mix_Chunk* wav = NULL;
public:
	sound();
	~sound();
	void playSound();
};