#pragma warning
#include "Game.h"
#define _CRTDBG_MAP_ALLOC
#include "EnumID.h"

Gamestate::ID Gamestate::StateOfGame::state;

int main()
{
	Game* game = new Game();

	game->run();

	delete game;

	_CrtDumpMemoryLeaks();

	return 0;
}
