#pragma warning
#include "Game.h"
#define _CRTDBG_MAP_ALLOC

int main()
{
	Game game;
	game.run();
	
	_CrtDumpMemoryLeaks();

	return 0;
}
