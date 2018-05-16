#pragma once
#include <Windows.h>
#include <SFML/Audio.hpp>
#include <iostream>
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "glm\glm\glm.hpp"

class SoundMasterSFML
{
private:
	sf::SoundBuffer Buffer;
	sf::Listener Listener;
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
	void setPosition(glm::vec3 pos);
	void setListenerPos(glm::vec3 pos);
	void setRelativeToListener(bool relative);
	void setAttenuation(float atten);

	void loop(bool looping);



};