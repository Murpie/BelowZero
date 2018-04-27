#pragma once
#include "al.h"
#include "alc.h"
#include <string>
#include <vector>

class SoundMaster
{
public:
	SoundMaster();
	SoundMaster(const std::string &filePath);

	~SoundMaster();

	void loadSource(const std::string & filePath);



private:



};
