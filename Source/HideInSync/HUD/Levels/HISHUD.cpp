// Fill out your copyright notice in the Description page of Project Settings.


#include "HISHUD.h"
#include "GameFramework/PlayerController.h"							// not sure when this is used, but is included in the course!
#include "CharacterOverlay.h"
#include "HideInSync/HUD/Levels/ScoreTextWidget.h"


void AHISHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void AHISHUD::DrawHUD()
{
	Super::DrawHUD();
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

void AHISHUD::InitScorePanel(int NumberOfPlayers)
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if ((PlayerController && ScoreTextWidgetClass) == false)
	{
		return;
	}

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < NumberOfPlayers - 1; ++j)
		{
			UScoreTextWidget* ScoreText = CreateWidget<UScoreTextWidget>(PlayerController, ScoreTextWidgetClass);
			CharacterOverlay->AddTextToScorePanel(ScoreText, i, j);
		}
	}
}
