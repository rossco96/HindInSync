// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include "Components/TextBlock.h"

void UCharacterOverlay::UpdateTimer(float ElapsedSeconds)
{
	int TotalSecondsInt = FMath::FloorToInt(ElapsedSeconds);
	int Minutes = TotalSecondsInt / 60;
	int Seconds = TotalSecondsInt % 60;

	//UE_LOG(LogActor, Warning, TEXT("[UCharacterOverlay::UpdateTimer] ElapsedSeconds / TotalSecondsInt / Minutes / Seconds --> %d / %d / %d / %d"), ElapsedSeconds, TotalSecondsInt, Minutes, Seconds);
	
	// Is below the best way to do this? Need to learn basic C++ functions...
	
	//FString TimeString = (Minutes < 10) ? "0" : "";
	//TimeString += FString::FromInt(Minutes) + ":";
	//TimeString += (Seconds < 10) ? "0" : "";
	//TimeString += FString::FromInt(Seconds);
	
	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	GameTimer->SetText(FText::FromString(TimeString));
}
