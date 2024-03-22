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
	int RespawnOrderId = -1;
	
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
