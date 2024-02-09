// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "HideInSync/PlayerController/LobbyPlayerController.h"


#pragma region Login / Logout
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	switch (NumberOfPlayers)
	{
	case 1:
		HostController = Cast<ALobbyPlayerController>(NewPlayer);
		break;
	case 2:
		// Enable ButtonStartGame here (this IF should never be required)
		if (HostController)
		{
			HostController->HostSetHUDButtonStartGameEnabled(true);
		}
		break;
	default:
		break;
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	// [TODO][Important]
	// Need to check if Exiting controller is host!
	// If so, will then have to set HostController to the first-joining client
	// And also replace their LobbyOverlayClient with the LobbyOverlayHost
	// (will want the existing options kept during this, but that should happen automatically anyway?)

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 1)
	{
		if (HostController)
		{
			// Disable ButtonStartGame here
			HostController->HostSetHUDButtonStartGameEnabled(false);
		}
		else
		{
			// else is only here for debug! Otherwise, merge the two IFs
		}
	}

	Super::Logout(Exiting);
}
#pragma endregion


#pragma region Blueprint Functions
void ALobbyGameMode::StartGame(FString LevelPath, FString GameModePath)
{
	UWorld* World = GetWorld();
	if (World)
	{
		bUseSeamlessTravel = true;
		
		//World->ServerTravel(FString("/Game/Maps/Test/HISMap?listen"));
		// '?listen' option above specifies that it'll be open as a listen server for clients to connect to
		
		// [TODO][IMPORTANT] Proof of concept -- won't always want listen server?
		FString FullPath = LevelPath + "?listen?game=" + GameModePath;

		UE_LOG(LogActor, Warning, TEXT("[asdf] %s"), *FullPath);

		World->ServerTravel(FullPath);
	}
}
#pragma endregion
