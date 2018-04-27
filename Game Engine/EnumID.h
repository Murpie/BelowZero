#pragma once

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
	static ID stateOfGame;
}

namespace Asset
{
	enum ID
	{
		Tree,
		Bucket
	};
}

namespace Scene
{
	enum ID
	{
		MENU,
		LEVEL_1
	};
}

namespace ComponentType
{
	enum ID
	{
		COMPONENT,
		PLAYER,
		LIGHT,
		TERRAIN
	};
}

namespace ObjectType
{
	enum ID
	{
		PLAYER,
		BUCKET,
		STONE_1,
		AXE,
		WOOD,
		MEAT,
		TREE
	};
}