// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "HideInSync/HUD/Levels/ScoreTextWidget.h"

void UCharacterOverlay::UpdateTimerLabel(FString Label)
{
	if (GameTimerLabel)
	{
		FText LabelText = FText::FromString(Label);
		GameTimerLabel->SetText(LabelText);
	}
}

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

void UCharacterOverlay::AddTextToScorePanel(class UWidget* ScoreText, int PositionX, int PositionY)
{
	ScorePanel->AddChildToUniformGrid(ScoreText, PositionX, PositionY);
}

void UCharacterOverlay::UpdateScore(int SlotNumber, int NewScore)
{
	UWidget* ScorePanelChild = ScorePanel->GetChildAt(SlotNumber);
	UScoreTextWidget* ScoreTextWidget = Cast<UScoreTextWidget>(ScorePanelChild);
	ScoreTextWidget->UpdateScore(NewScore);
}
