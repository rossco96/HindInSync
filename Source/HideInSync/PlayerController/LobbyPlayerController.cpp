// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "HideInSync/HUD/Menus/LobbyHUD.h"
#include "HideInSync/HUD/Menus/LobbyOverlayHost.h"

void ALobbyPlayerController::HostSetHUDButtonStartGameEnabled(bool Enabled)
{
	ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(GetHUD());
	if (LobbyHUD)
	{
		ULobbyOverlayHost* LobbyOverlayHost = Cast<ULobbyOverlayHost>(LobbyHUD->LobbyOverlay);
		if (LobbyOverlayHost)
		{
			LobbyOverlayHost->SetButtonStartGameEnabled(Enabled);
		}
	}
}
