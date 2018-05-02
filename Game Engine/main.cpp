#pragma warning
#include "Game.h"
#define _CRTDBG_MAP_ALLOC
#include "EnumID.h"

Gamestate::ID Gamestate::StateOfGame::state;

int main()
{
	Game game;
	game.run();
	
	_CrtDumpMemoryLeaks();

	return 0;
}
