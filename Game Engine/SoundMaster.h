#pragma once
#include <Windows.h>
#include <vector>
#include <SFML\Audio.hpp>
#include <SFML\System.hpp>
#include <SFML\Main.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\Audio\Sound.hpp>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\Audio\SoundBufferRecorder.hpp>
#include <iostream>

class SoundMasterSFML
{
private:
	sf::SoundBuffer Buffer;
	sf::Sound Sound;

	unsigned int SampleRate;
	unsigned int Channels;
	sf::Time Duration;
	const sf::Int16* Samples;


public:

	SoundMasterSFML();
	SoundMasterSFML(const std::string & filePath);

	~SoundMasterSFML();

	void addSound(const std::string & filePath);

	void playSound();
	void pauseSound();
	void stopSound();
	void setVolume(float volume);
	void setPitch(float pitch);

	void loop(bool looping);



};