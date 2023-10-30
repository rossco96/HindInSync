// Fill out your copyright notice in the Description page of Project Settings.


#include "InLevelGameMode.h"
#include "GameFramework/GameStateBase.h"							// [TODO] GameStateBase needed for GameState.Get() ONLY
//#include "HideInSync/Character/HISCharacterClone.h"

void AInLevelGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	for (int i = 0; i < WinConditions.Num(); ++i)
	{
		if (WinConditions[i].CheckCondition())
		{
			// END THE GAME!
			return;
		}
	}
	//*/
}


// PostLogin is the first place where it's safe to access the player that just joined the game
void AInLevelGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 PlayerId = GameState.Get()->PlayerArray.Num() - 1;
	//HISCharacterClone Player = (HISCharacterClone)PlayerArray[PlayerId];					// [TODO] Should use pointer here, no?

	// [TODO] want to assign this new player to PlayersData
	// zero-based indexing please!
	//PlayersData.Add(PlayerId, PlayerGameData(PlayerId));									// OLD WAY OF DOING THIS

	// [TODO][IMPORTANT] Also need to assign a reference on the player themselves, too!


	
	// [TODO] want to start countdown once target reached
	/*
	if (PlayerId + 1 == TargetNumberOfPlayers)
	{
		// [TODO] Set HISCharacterClone PlayerId
		// (and assigned PlayerStart..? Can we do that here???)
		// 
		// [TODO] Call to initialise all player arrays
		// 
		// [TODO] Move to HIDE gamestate, with countdown
	}
	//*/
}


/*
void AInLevelGameMode::SetWinConditions(TArray<WinCondition> Conditions)
{
	WinConditions = Conditions;
}
//*/


void AInLevelGameMode::FindPlayer(int SeekerId, int HiderId)
{
	PlayersData[SeekerId].AddToPlayerFound(HiderId);
	PlayersData[HiderId].AddToFoundByPlayer(SeekerId);
}
