// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGameData.h"
#include "HideInSync/PlayerController/HISPlayerController.h"


#pragma region Constructors
PlayerGameData::PlayerGameData(class AHISPlayerController* Controller)
{
	HISPlayerController = Controller;
}

PlayerGameData::~PlayerGameData()
{
}
#pragma endregion // Constructors


#pragma region Hide Position
void PlayerGameData::SetSpawnPosition(FVector Location, FRotator Rotation)
{
	if (bSpawnPositionSet)
	{
		UE_LOG(LogActor, Warning, TEXT("[PlayerGameData::SetSpawnPosition] (already set, early return)"));
		return;
	}

	UE_LOG(LogActor, Warning, TEXT("[PlayerGameData::SetSpawnPosition] bSpawnPositionSet"));
	SpawnLocation = Location;
	SpawnRotation = Rotation;
	bSpawnPositionSet = true;
}

void PlayerGameData::ResetSpawn()
{
	bSpawnPositionSet = false;
}


void PlayerGameData::SetHidePosition(FVector Location, FRotator Rotation)
{
	UE_LOG(LogActor, Warning, TEXT("[PlayerGameData::SetHidePosition] ~~~HIDINGPLAYER~~~"));
	HideLocation = Location;
	HideRotation = Rotation;
	bHidingPlaceSet = true;
}
#pragma endregion
