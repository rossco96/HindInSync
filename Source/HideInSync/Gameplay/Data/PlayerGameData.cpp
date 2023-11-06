// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGameData.h"
#include "HideInSync/PlayerController/HISPlayerController.h"


#pragma region Constructors & Init
PlayerGameData::PlayerGameData(class AHISPlayerController* Controller)
{
	HISPlayerController = Controller;
}

PlayerGameData::~PlayerGameData()
{
}


// [TODO] Is there a way to not pass TotalPlayers? Get via InLevelGameMode? Or is this the easiest/cleanest solution?
void PlayerGameData::InitFoundArrays(int TotalPlayers)
{
	for (int i = 0; i < TotalPlayers; ++i)
	{
		if (i == HISPlayerController->GetPlayerId()) continue;
		PlayersFound.Add(i, 0);
		FoundByPlayers.Add(i, 0);
	}
}
#pragma endregion // Constructors & Init


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

void PlayerGameData::SetHidePosition(FVector Location, FRotator Rotation)
{
	HideLocation = Location;
	HideRotation = Rotation;
	bIsHidden = true;
}
#pragma endregion


#pragma region Found Array Functions
int PlayerGameData::GetPlayerFoundAtIndex(int Id)
{
	return PlayersFound[Id];
}

int PlayerGameData::GetFoundByPlayerAtIndex(int Id)
{
	return FoundByPlayers[Id];
}


void PlayerGameData::AddToPlayerFound(int Id)
{
	PlayersFound[Id]++;
}

void PlayerGameData::AddToFoundByPlayer(int Id)
{
	FoundByPlayers[Id]++;
}
#pragma region // Found Array Functions
