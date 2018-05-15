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
		MAINMENU,
		MESHFILTER
	};
}

namespace ObjectType
{
	enum ID
	{
		Player,
		Bucket_Empty,
		Stone_1,
		Campfire,
		Cliffside_1,
		Cliffside_2,
		Cliffside_3,
		Cliffside_4,
		Campfire_NoSnow,
		WoodenSign,
		Pine_Tree,
		Pine_Tree_Snow,
		BrokenTree,
		BrokenTree_Snow,
		IceTaps_1,
		IceTaps_2,
		WaterBottle,
		Flag,
		Can,
		Bucket_Water,
		Bucket_Snow,
		Stump,
		Tree,
		TreeWithSnow,
		FlareGunBox,
		Axe,
		Jacket,
		Stone_2,
		Stone_3,
		Stone_4,
		Stone_5,
		Cabin,
		DeadTree,
		Equiped_Axe,
		EquipedEmptyBucket,
		EquipedEmptyBucketSnow,
		EquipedEmptyBucketWater,
		Tree_Small,
		DeadTree_Small,
		DeadTreeSnow,
		DeadTreeSnow_Small,
		Tree_Small_Snow,
		FlareGun,
		IceWall,
		EquipedLighter
	};
}