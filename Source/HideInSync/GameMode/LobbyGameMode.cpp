// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "HideInSync/PlayerController/LobbyPlayerController.h"


#pragma region Login / Logout
// PostLogin is the first place where it's safe to access the player that just joined the game
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	switch (NumberOfPlayers)
	{
	case 1:
		HostController = Cast<ALobbyPlayerController>(NewPlayer);
		//HostController->ClientSetHUD()
		// Disable ButtonStartGame here? Or is it disabled by default?
		break;
	case 2:
		// Enable ButtonStartGame here
		if (HostController)											// This IF should never be required
		{
			HostController->HostSetHUDButtonStartGameEnabled(true);
		}
		break;
	default:
		break;
	}

	return;

	if (NumberOfPlayers == 2)
	{

	}

	if (NumberOfPlayers == 3)
	//if (NumberOfPlayers == 2)											// [TODO] Temp hardcoded '2' --> Will want a button to start the game? Unless public match ... Event then, could use request start (which begins say 2 min countdown so trolls can't just like never begin the match... That or just have a timer once a minimum number of players reached?)
	{
		StartGame();
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
			if (HostController)
			{
				HostController->HostSetHUDButtonStartGameEnabled(false);
			}
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
void ALobbyGameMode::StartGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		bUseSeamlessTravel = true;
		World->ServerTravel(FString("/Game/Maps/Test/HISMap?listen"));	// '?listen' option specifies that it'll be open as a listen server for clients to connect to
	}
}

void ALobbyGameMode::AddWidgetToOwningClient(class UWidget* Widget)
{

}
#pragma endregion
