// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyHUD.h"
#include "LobbyOverlayBase.h"
//#include "Net/UnrealNetwork.h"

/*
void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();



	//AddLobbyOverlay(true);
}
//*/


void ALobbyHUD::ClientAddLobbyOverlay_Implementation(bool IsHost)
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && LobbyOverlayHostClass)
	{
		if (IsHost && LobbyOverlayHostClass)
		{
			LobbyOverlay = CreateWidget<class ULobbyOverlayHost>(PlayerController, LobbyOverlayHostClass);
		}
		else if (IsHost == false && LobbyOverlayClientClass)
		{
			LobbyOverlay = CreateWidget<class ULobbyOverlayClient>(PlayerController, LobbyOverlayClientClass);
		}
		LobbyOverlay->AddToViewport();
	}
}
