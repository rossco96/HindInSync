// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyOverlayHost.h"
#include "Components/Button.h"

void ULobbyOverlayHost::SetButtonStartGameEnabled(bool Enabled)
{
	ButtonStartGame->SetIsEnabled(Enabled);
}
