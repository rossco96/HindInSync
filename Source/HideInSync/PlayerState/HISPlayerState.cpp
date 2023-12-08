// Fill out your copyright notice in the Description page of Project Settings.


#include "HISPlayerState.h"
#include "HideInSync/Character/HISCharacter.h"
#include "HideInSync/PlayerController/HISPlayerController.h"

// [NOTE] Should only call this on the server!
void AHISPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;
	HISCharacter = (HISCharacter == nullptr) ? Cast<AHISCharacter>(GetPawn()) : HISCharacter;
	if (HISCharacter)
	{
		HISPlayerController = (HISPlayerController == nullptr) ? Cast<AHISPlayerController>(HISCharacter->Controller) : HISPlayerController;
		if (HISPlayerController)
		{
			HISPlayerController->SetHUDScore(Score);
		}
	}
}


// [NOTE] This function is never called on the server!
void AHISPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	HISCharacter = (HISCharacter == nullptr) ? Cast<AHISCharacter>(GetPawn()) : HISCharacter;
	if (HISCharacter)
	{
		HISPlayerController = (HISPlayerController == nullptr) ? Cast<AHISPlayerController>(HISCharacter->Controller) : HISPlayerController;
		if (HISPlayerController)
		{
			HISPlayerController->SetHUDScore(Score);
		}
	}
}
