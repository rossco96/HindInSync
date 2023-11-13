// Fill out your copyright notice in the Description page of Project Settings.


#include "HISHUD.h"
#include "GameFramework/PlayerController.h"							// not sure when this is used, but is included in the course!
#include "CharacterOverlay.h"

void AHISHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void AHISHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AHISHUD::DrawHUD()
{
	Super::DrawHUD();
}
