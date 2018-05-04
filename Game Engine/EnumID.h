#pragma once
#define _CRTDBG_MAP_ALLOC
#define SCREEN_WIDTH 1280.0
#define SCREEN_HEIGHT 720.0

namespace Gamestate
{
	enum ID
	{
		INITIALIZE, 
		LOAD_MENU,
		SHOW_MENU,
		CLEAR_MENU,
		LOAD_LEVEL,
		RUN_LEVEL,
		CLEAR_LEVEL,
		CLOSE_GAME
	};
	struct StateOfGame
	{
		static ID state;
	};
}

namespace Scene
{
	enum ID
	{
		EMPTY,
		MENU,
		LEVEL_1,
		LEVEL_2
	};
}

namespace ComponentType
{
	enum ID
	{
		PLAYER,
		COMPONENT,
		TERRAIN,
		LIGHT,
		MATERIAL,
		MAINMENU
	};
}

namespace ObjectType
{
	enum ID
	{
		PLAYER,
		BUCKET,
		STONE_1,
		CAMPFIRE,
		AXE,
		WOOD,
		MEAT,
		TREE
	};
}