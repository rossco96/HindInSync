#pragma once

#include "CoreMinimal.h"



// GameMode Enum below: Names of all the possible game modes
// (as called by their class NAMES, e.g. "FK" rather than "FindersKeepers")
UENUM(BlueprintType)
enum class EGameMode : uint8
{
	AON			UMETA(DisplayName = "All or Nothing"),
	FK			UMETA(DisplayName = "Finders Keepers"),
	LCS			UMETA(DisplayName = "Last Clone Standing"),
	Classic		UMETA(DisplayName = "Classic (non-sync)"),
	Sardines	UMETA(DisplayName = "Sardines (non-sync)")
};



// [TODO][Q] Consider moving this elsewhere? Do I want a separate ENUMS .cpp file??
// [TODO][IMPORTANT] Make sure .umap names and display names all make sense!
// E.g. previously "Desert" = LevelMap_Egypt.umap (not a great idea!)
// 
// LevelName Enum below: Names of all the possible game modes
// (as called by their map NAMES, e.g. "" rather than "FindersKeepers")
UENUM(BlueprintType)
enum class ELevelName : uint8
{
	TEST				UMETA(DisplayName = "TEST"),
	LevelMap_Desert		UMETA(DisplayName = "Desert")
};



// Respawn Enum below: Need to keep track of which stage of respawn each player is moving onto
UENUM()
enum class ERespawnState : uint8
{
	Found,
	RespawnHiderWait,								// [TODO][IMPORTANT] Get actual people testing this for fun aspect! May want to not have any wait at all between hiding and seeking respawning
	Hider,
	RespawnSeekerWait,								// [TODO][IMPORTANT] (see above)
	NONE
};



// Below to be ignored?
class GameplayEnums
{
};

