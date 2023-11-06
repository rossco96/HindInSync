// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "HideInSync/HUD/Menus/LobbyHUD.h"
#include "HideInSync/HUD/Menus/LobbyOverlayHost.h"

void ALobbyPlayerController::HostSetHUDButtonStartGameEnabled(bool Enabled)
{
	ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(GetHUD());
	if (LobbyHUD)
	{
		// If we're in this function then we must be the host - no need to nullcheck LobbyOverlayHost
		ULobbyOverlayHost* LobbyOverlayHost = Cast<ULobbyOverlayHost>(LobbyHUD->LobbyOverlay);
		if (LobbyOverlayHost)
		{
			LobbyOverlayHost->SetButtonStartGameEnabled(Enabled);
		}
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[ALobbyPlayerController::SetHUDButtonStartGameEnabled] LobbyHUD == nullptr ... major issue! (break)"));
	}
}
