#pragma once

#include "al.h"
#include "alc.h"
#include <string>
#include <vector>

class Sound
{
public:
	Sound();
	~Sound();

	bool isPlaying;

	void addSoundFile(const std::string &filename);
	void play(ALuint source);
	void pause();
	void stop();

	std::vector<ALuint> sources;


	ALCcontext *context;
	ALCdevice *device;
	ALuint sBuffer[32];
	ALuint source[32];



private:


};