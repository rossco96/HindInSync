// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class HIDEINSYNC_API PlayerGameData
{
public:
	PlayerGameData(class AHISPlayerController* Controller);
	//PlayerGameData(int Id);
	~PlayerGameData();

	// [TODO] See note above function in .cpp
	void InitFoundArrays(int TotalPlayers);

	FORCEINLINE AHISPlayerController* GetController() { return HISPlayerController; }

	FORCEINLINE bool IsPlayerHidden() { return bIsHidden; }
	//FORCEINLINE void SetPlayerHidden(bool bHidden) { bIsHidden = bHidden; }		// [TODO] Delete this method! Set 'bIsHidden = true' in SetHidePosition ... Figure out where to set it false!

	FORCEINLINE FVector GetSpawnLocation() { return SpawnLocation; }
	FORCEINLINE FRotator GetSpawnRotation() { return SpawnRotation; }
	void SetSpawnPosition(FVector Location, FRotator Rotation);

	FORCEINLINE FVector GetHideLocation() { return HideLocation; }
	FORCEINLINE FRotator GetHideRotation() { return HideRotation; }
	void SetHidePosition(FVector Location, FRotator Rotation);

	int GetPlayerFoundAtIndex(int Id);
	int GetFoundByPlayerAtIndex(int Id);
	void AddToPlayerFound(int Id);
	void AddToFoundByPlayer(int Id);

private:
	//int PlayerId;
	class AHISPlayerController* HISPlayerController;
	bool bSpawnPositionSet = false;
	bool bIsHidden = false;

	FVector SpawnLocation;
	FRotator SpawnRotation;
	FVector HideLocation;
	FRotator HideRotation;

	TMap<int, int> FoundByPlayers;
	TMap<int, int> PlayersFound;
};
