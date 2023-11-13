// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include "Components/TextBlock.h"

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
