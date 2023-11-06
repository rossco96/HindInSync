// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include "Components/TextBlock.h"

void UCharacterOverlay::UpdateTimer(int ElapsedSeconds)
{
	int Minutes = ElapsedSeconds / 60;
	int Seconds = ElapsedSeconds % 60;
	//UE_LOG(LogActor, Warning, TEXT("[UCharacterOverlay::UpdateTimer] ElapsedSeconds / TotalSecondsInt / Minutes / Seconds --> %d / %d / %d / %d"), ElapsedSeconds, TotalSecondsInt, Minutes, Seconds);
	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	GameTimer->SetText(FText::FromString(TimeString));
}
