#include "SoundMaster.h"

SoundMasterSFML::SoundMasterSFML()
{

}

SoundMasterSFML::SoundMasterSFML(const std::string & filePath)
{
	addSound(filePath);
}

SoundMasterSFML::~SoundMasterSFML()
{
	//delete &this->Buffer;
	//delete &this->Sound;
}

void SoundMasterSFML::addSound(const std::string & filePath)
{
	if (!this->Buffer.loadFromFile(filePath))
		std::cout << "Cant Load File" << std::endl;

	Sound.setBuffer(Buffer);
}

bool SoundMasterSFML::isPlaying()
{
	if (Sound.getStatus() == 2)
		return true;

	return false;
}

void SoundMasterSFML::playSound()
{
	Sound.play();
}

void SoundMasterSFML::pauseSound()
{
	Sound.pause();
}

void SoundMasterSFML::stopSound()
{
	Sound.stop();
}

void SoundMasterSFML::setVolume(float volume)
{
	Sound.setVolume(volume);
}

void SoundMasterSFML::setPitch(float pitch)
{
	Sound.setPitch(pitch);

}

void SoundMasterSFML::loop(bool looping)
{
	Sound.setLoop(looping);
}
