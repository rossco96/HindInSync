// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

// PostLogin is the first place where it's safe to access the player that just joined the game
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 2)											// [TODO] Temp hardcoded '2'
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/Test/HISMap?listen"));	// '?listen' option specifies that it'll be open as a listen server for clients to connect to
		}
	}
}
