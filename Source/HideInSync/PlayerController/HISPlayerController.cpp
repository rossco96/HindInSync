// Fill out your copyright notice in the Description page of Project Settings.


#include "HISPlayerController.h"
#include "GameFramework/Character.h"
#include "HideInSync/HUD/Levels/HISHUD.h"
#include "Net/UnrealNetwork.h"


void AHISPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHISPlayerController, PlayerId);
}


#pragma region Client RPC
void AHISPlayerController::ClientUpdateHUDTimer_Implementation(int Seconds)
{
	// [TODO][IMPORTANT]
	// Should store this at the very start! Do not call Cast on Tick!
	// Then should not need debug else statement below
	AHISHUD* HISHUD = Cast<AHISHUD>(GetHUD());
	if (HISHUD)
	{
		if (HISHUD->CharacterOverlay)
		{
			HISHUD->CharacterOverlay->UpdateTimer(Seconds);
		}
	}
}
#pragma endregion
