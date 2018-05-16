#pragma once
#include <Windows.h>
#include <SFML/Audio.hpp>
#include <iostream>

class SoundMasterSFML
{
private:
	sf::SoundBuffer Buffer;
	sf::Sound Sound;

public:

	SoundMasterSFML();
	SoundMasterSFML(const std::string & filePath);

	~SoundMasterSFML();

	void addSound(const std::string & filePath);

	bool isPlaying();
	void playSound();
	void pauseSound();
	void stopSound();
	void setVolume(float volume);
	void setPitch(float pitch);

	void loop(bool looping);



};