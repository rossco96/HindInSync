// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"

void UCharacterOverlay::UpdateTimer(int Seconds)
{
	if (GameTimer)
	{
		int MinutesDiv = Seconds / 60;
		int SecondsMod = Seconds % 60;
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), MinutesDiv, SecondsMod);
		GameTimer->SetText(FText::FromString(TimeString));
	}
}

void UCharacterOverlay::SetFoundTextVisible(bool bIsVisible)
{
	ESlateVisibility TextVisibility = (bIsVisible) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden;
	FoundText->SetVisibility(TextVisibility);
}

void UCharacterOverlay::InitScorePanel(int NumberOfOtherPlayers)
{
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < NumberOfOtherPlayers; ++j)
		{
			ScorePanel->AddChildToUniformGrid(TextBlockClass, i, j);
		}
	}
}

void UCharacterOverlay::AddToScore(int PlayerId, bool bAreWeTheSeeker)
{
	int RowNumber = (bAreWeTheSeeker) ? 0 : 1;
	//ScorePanel->
}
