// Fill out your copyright notice in the Description page of Project Settings.


#include "HISHUD.h"
#include "CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"							// not sure when this is used, but is included in the course!
#include "HideInSync/HUD/Levels/AnnouncementWidget.h"
#include "HideInSync/HUD/Levels/ScoreTextWidget.h"
#include "HideInSync/PlayerController/HISPlayerController.h"


void AHISHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AHISHUD::DrawHUD()
{
	Super::DrawHUD();
}


void AHISHUD::InitScorePanel(int NumberOfPlayers)
{
	AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(GetOwningPlayerController());
	if ((HISPlayerController && ScoreTextWidgetClass) == false)
	{
		return;
	}

	int PlayerId = HISPlayerController->GetPlayerId();

	// Set the headers, ignoring the first square
	for (int i = 0; i < NumberOfPlayers; ++i)
	{
		if (PlayerId == i) continue;
		int HeaderIndex = (i < PlayerId) ? i + 1 : i;
		UScoreTextWidget* ScoreTextName = CreateWidget<UScoreTextWidget>(HISPlayerController, ScoreTextWidgetClass);
		FString NameString = FString("Player_") + FString::FromInt(i);	// Not sure this is ideal, and will need to be replaced with actual player names!
		ScoreTextName->SetTextFromString(NameString);
		CharacterOverlay->AddTextToScorePanel(ScoreTextName, 0, HeaderIndex);
	}

	// fill in the remaining rows
	for (int Row = 1; Row <= 2; ++Row)
	{
		for (int Col = 0; Col < NumberOfPlayers; ++Col)
		{
			UScoreTextWidget* ScoreText = CreateWidget<UScoreTextWidget>(HISPlayerController, ScoreTextWidgetClass);
			if (Col == 0)
			{
				// add FOUND and FOUND BY text if first column
				FString FoundString = FString("FOUND");
				if (Row == 2)
				{
					FoundString += FString(" BY");
				}
				ScoreText->SetTextFromString(FoundString);
			}
			CharacterOverlay->AddTextToScorePanel(ScoreText, Row, Col);
		}
	}
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

void AHISHUD::AddAnnouncementWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementWidgetClass)
	{
		AnnouncementWidget = CreateWidget<UAnnouncementWidget>(PlayerController, AnnouncementWidgetClass);
		AnnouncementWidget->AddToViewport();
	}
}
