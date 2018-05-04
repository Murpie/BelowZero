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
}

void SoundMasterSFML::addSound(const std::string & filePath)
{
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("Snow.wav"))
		return;

	if (!this->Buffer.loadFromFile(filePath.c_str()))
		std::cout << "Cant Load File" << std::endl;

	this->SampleRate = this->Buffer.getSampleRate();
	this->Channels = this->Buffer.getChannelCount();
	this->Duration = this->Buffer.getDuration();
	this->Samples = this->Buffer.getSamples();

	Sound.setBuffer(Buffer);
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
