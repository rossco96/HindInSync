// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerGameData.generated.h"

// Respawn Enum below: Need to keep track of which stage of respawn each player is moving onto
UENUM()
enum class ERespawnState : uint8
{
	RSE_Found,
	RSE_RespawnHiderWait,								// [TODO][IMPORTANT] Test this! May want to not have any wait at all between hiding and seeking respawning
	RSE_Hider,
	RSE_RespawnSeekerWait,								// [TODO][IMPORTANT] (see above)
	RSE_NONE
};

/**
 * 
 */
class HIDEINSYNC_API PlayerGameData
{
public:
	PlayerGameData(class AHISPlayerController* Controller);
	~PlayerGameData();

	// [TODO] See note above function in .cpp
	void InitFoundArrays(int TotalPlayers);

	FORCEINLINE AHISPlayerController* GetController() { return HISPlayerController; }
	FORCEINLINE FTimerHandle GetWaitTimer() { return WaitTimer; }

	FORCEINLINE bool IsPlayerHidden() { return bIsHidden; }				// Are we actually using this?

	FORCEINLINE FVector GetSpawnLocation() { return SpawnLocation; }
	FORCEINLINE FRotator GetSpawnRotation() { return SpawnRotation; }
	void SetSpawnPosition(FVector Location, FRotator Rotation);
	void ResetSpawn();

	FORCEINLINE FVector GetHideLocation() { return HideLocation; }
	FORCEINLINE FRotator GetHideRotation() { return HideRotation; }
	void SetHidePosition(FVector Location, FRotator Rotation);
	void ResetHide();

	int GetPlayerFoundAtIndex(int Id);
	int GetFoundByPlayerAtIndex(int Id);
	void AddToPlayerFound(int Id);
	void AddToFoundByPlayer(int Id);

	ERespawnState RespawnState = ERespawnState::RSE_NONE;

private:
	class AHISPlayerController* HISPlayerController;					// Do I need to forward declare 'class' here?
	FTimerHandle WaitTimer;
	
	//bool bPlayerLoadedIn = false;										// Need this?
	bool bSpawnPositionSet = false;
	bool bIsHidden = false;												// Are we actually using this?

	FVector SpawnLocation;
	FRotator SpawnRotation;
	FVector HideLocation;
	FRotator HideRotation;

	TMap<int, int> FoundByPlayers;
	TMap<int, int> PlayersFound;
};
