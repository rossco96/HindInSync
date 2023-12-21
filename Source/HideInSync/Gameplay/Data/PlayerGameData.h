// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerGameData.generated.h"

// Respawn Enum below: Need to keep track of which stage of respawn each player is moving onto
UENUM()
enum class ERespawnState : uint8
{
	RSE_Found,
	RSE_RespawnHiderWait,								// [TODO][IMPORTANT] Get actual people testing this for fun aspect! May want to not have any wait at all between hiding and seeking respawning
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

	FORCEINLINE AHISPlayerController* GetController() { return HISPlayerController; }
	FORCEINLINE FTimerHandle* GetWaitTimer() { return &WaitTimer; }

	FORCEINLINE FVector GetSpawnLocation() { return SpawnLocation; }
	FORCEINLINE FRotator GetSpawnRotation() { return SpawnRotation; }
	void SetSpawnPosition(FVector Location, FRotator Rotation);
	void ResetSpawn();

	FORCEINLINE FVector GetHideLocation() { return HideLocation; }
	FORCEINLINE FRotator GetHideRotation() { return HideRotation; }
	void SetHidePosition(FVector Location, FRotator Rotation);

	ERespawnState RespawnState = ERespawnState::RSE_NONE;
	bool bCheckRespawnState = false;									// [TODO] IMPLEMENT! Think this will be needed --> e.g. P2 is 10s out of 30s into the hiding state and then P3 enters the hiding state, so when iterating to find which player is in the hiding state will come up to P2 first (if not using this bool)
	
	bool bIsHidden = false;

private:
	class AHISPlayerController* HISPlayerController;					// Do I need to forward declare 'class' here?
	FTimerHandle WaitTimer;
	
	bool bSpawnPositionSet = false;

	FVector SpawnLocation;
	FRotator SpawnRotation;
	FVector HideLocation;
	FRotator HideRotation;
};
