#include "sound.h"
#include <SDL_mixer.h>
#include <SDL.h>

sound::sound()
{
	SDL_Init(SDL_INIT_AUDIO);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	wav = Mix_LoadWAV(WAV_PATH);

	Mix_Volume(-1, MIX_MAX_VOLUME / 4);
}

sound::~sound()
{
	Mix_FreeChunk(wav);
	Mix_CloseAudio();
}

void sound::playSound()
{
	Mix_PlayChannel(-1, wav, 0);
}