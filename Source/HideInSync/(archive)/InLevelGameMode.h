// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
//#include "HideInSync/Gameplay/WinConditions/WinCondition.h"
#include "HideInSync/Gameplay/Data/PlayerGameData.h"
#include "InLevelGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API AInLevelGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

	// PostLogin is the first place where it's safe to access the player that just joined the game
	virtual void PostLogin(APlayerController* NewPlayer) override;

	//void SetWinConditions(TArray<WinCondition> WinConditions);
	void FindPlayer(int SeekerId, int HiderId);

private:
	//TArray<WinCondition> WinConditions;
	TMap<int, PlayerGameData> PlayersData;
};
