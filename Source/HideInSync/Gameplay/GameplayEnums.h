#pragma once

#include "CoreMinimal.h"


#pragma region GAME SETTINGS
// GameMode Enum below: Names of all the possible game modes
// (as called by their class NAMES, e.g. "FK" rather than "FindersKeepers")
UENUM(BlueprintType)
enum class EGameMode : uint8
{
	AON					UMETA(DisplayName = "All or Nothing"),
	FK					UMETA(DisplayName = "Finders Keepers"),
	LCS					UMETA(DisplayName = "Last Clone Standing"),
	Classic				UMETA(DisplayName = "Classic (non-sync)"),
	Sardines			UMETA(DisplayName = "Sardines (non-sync)")
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


// GameTimeLimit Enum below, in minutes
// [TODO] Should have timer counting UP if selecting "Endless"
UENUM(BlueprintType)
enum class EGameTimeLimit : uint8
{
	T2					UMETA(DisplayName = "2"),
	T5					UMETA(DisplayName = "5"),
	T10					UMETA(DisplayName = "10"),
	T15					UMETA(DisplayName = "15"),
	T0					UMETA(DisplayName = "Endless")		// Think of a better word than 'endless' ?
};


// HideTimeLimit Enum below, in seconds
// [TODO][Q] Should have timer counting UP if selecting "Endless"? Or just show the actual remaining GameTime?
UENUM(BlueprintType)
enum class EHideTimeLimit : uint8
{
	T15					UMETA(DisplayName = "15"),
	T30					UMETA(DisplayName = "30"),
	T60					UMETA(DisplayName = "60"),
	T120				UMETA(DisplayName = "120"),
	T0					UMETA(DisplayName = "Endless")		// Think of a better word than 'endless' ?
};


// [TODO] Eventually delete this? Or settle on a specific time and then can either have just RespawnWait "on" or "off"
// RespawnWait Enum below, in seconds
UENUM(BlueprintType)
enum class ERespawnWait : uint8
{
	T0					UMETA(DisplayName = "0"),
	T3					UMETA(DisplayName = "3"),
	T5					UMETA(DisplayName = "5"),
	T10					UMETA(DisplayName = "10")
};
#pragma endregion


#pragma region STATES
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
#pragma endregion


#pragma region (ignore)
// Below to be ignored?
class GameplayEnums
{
};
#pragma endregion
