// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HideInSync/Gameplay/GameplayEnums.h"

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

	ERespawnState RespawnState = ERespawnState::NONE;
	bool bCheckRespawnState = false;									// [TODO] IMPLEMENT! Think this will be needed --> e.g. P2 is 10s out of 30s into the hiding state and then P3 enters the hiding state, so when iterating to find which player is in the hiding state will come up to P2 first (if not using this bool)
	
	bool bHidingPlaceSet = false;
	bool bIsHidden = false;

private:
	AHISPlayerController* HISPlayerController;
	FTimerHandle WaitTimer;
	
	bool bSpawnPositionSet = false;

	FVector SpawnLocation;
	FRotator SpawnRotation;
	FVector HideLocation;
	FRotator HideRotation;
};
