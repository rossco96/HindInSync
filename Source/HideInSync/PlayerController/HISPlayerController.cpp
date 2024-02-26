// Fill out your copyright notice in the Description page of Project Settings.


#include "HISPlayerController.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "HideInSync/HUD/Levels/CharacterOverlay.h"
#include "HideInSync/HUD/Levels/HISHUD.h"
#include "Net/UnrealNetwork.h"



// TEMP DEBUG ONLY BELOW
FVector AHISPlayerController::GetCurrentLocation()
{
	if (GetCharacter())
	{
		return GetCharacter()->GetTransform().GetLocation();
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::GetCurrentLocation] GetCharacter() is nullptr -- MAJOR ERROR"));
	}
	return FVector::ZeroVector;
}

FRotator AHISPlayerController::GetCurrentRotation()
{
	if (GetCharacter())
	{
		return GetCharacter()->GetTransform().GetRotation().Rotator();
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::GetCurrentRotation] GetCharacter() is nullptr -- MAJOR ERROR"));
	}
	return FRotator::ZeroRotator;
}
// TEMP DEBUG ONLY ABOVE


#pragma region Unreal Functions
void AHISPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHUDScoresInitialised)
	{
		return;
	}

	// [NOTE][HACK?][14.12.2023]
	if (IsLocalPlayerController() && bAllPlayersLoadedIn && TotalNumberOfPlayers != -1)		// Is the check for TotalNumberOfPlayers required? Use OnRep_TotalNumberOfPlayers() instead! Then no need for checking in Tick(), nor any need for bHUDScoresInitialised
	{
		HISHUD = Cast<AHISHUD>(GetHUD());
		if (HISHUD)
		{
			if (HISHUD->CharacterOverlay)
			{
				HISHUD->InitScorePanel(TotalNumberOfPlayers);
				bHUDScoresInitialised = true;
			}
		}
	}
}

void AHISPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHISPlayerController, PlayerId);
	DOREPLIFETIME(AHISPlayerController, bAllPlayersLoadedIn);
	DOREPLIFETIME(AHISPlayerController, TotalNumberOfPlayers);
}
#pragma endregion


#pragma region HUD
void AHISPlayerController::SetHUDScore(float Score)
{
	HISHUD = (HISHUD == nullptr) ? Cast<AHISHUD>(GetHUD()) : HISHUD;
	if (HISHUD && HISHUD->CharacterOverlay && HISHUD->CharacterOverlay->ScoreAmountText)		// [TODO] Make any others update like this! Timer???
	{
		FString ScoreAmount = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		HISHUD->CharacterOverlay->ScoreAmountText->SetText(FText::FromString(ScoreAmount));
	}
}
#pragma endregion


#pragma region Client RPC
void AHISPlayerController::ClientUpdateHUDTimerLabel_Implementation(ETimerLabelState TimerState)
{
	if (HISHUD)
	{
		if (HISHUD->CharacterOverlay)
		{
			// [TODO] HACK!!!!! This is a proof of concept ONLY!!!
			FName TimerStateName = UEnum::GetValueAsName<ETimerLabelState>(TimerState);
			FString LeftString = "";
			FString RightString = "";
			TimerStateName.ToString().Split("::", &LeftString, &RightString);
			FString TimerLabel = RightString + ":";

			HISHUD->CharacterOverlay->UpdateTimerLabel(TimerLabel);
		}
	}
}

void AHISPlayerController::ClientUpdateHUDTimer_Implementation(int Seconds)
{
	if (HISHUD)
	{
		if (HISHUD->CharacterOverlay)
		{
			HISHUD->CharacterOverlay->UpdateTimer(Seconds);
		}
		else
		{
			UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ClientUpdateHUDTimer_Implementation] CharacterOverlay is nullptr (but this is okay?)"));
		}
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ClientUpdateHUDTimer_Implementation] HISHUD is nullptr (NOT OKAY?)"));
	}
}

void AHISPlayerController::ClientSetFoundTextVisible_Implementation(bool bIsVisible)
{
	if (HISHUD)
	{
		if (HISHUD->CharacterOverlay)
		{
			HISHUD->CharacterOverlay->SetFoundTextVisible(bIsVisible);
		}
		else
		{
			UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ClientSetFoundTextEnabled_Implementation] CharacterOverlay is nullptr (but this is okay?)"));
		}
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ClientSetFoundTextEnabled_Implementation] HISHUD is nullptr (NOT OKAY?)"));
	}
}

void AHISPlayerController::ClientUpdateHUDScores_Implementation(int SlotNumber, int NewScore)
{
	if (HISHUD && HISHUD->CharacterOverlay)
	{
		HISHUD->CharacterOverlay->UpdateScore(SlotNumber, NewScore);
	}
}
#pragma endregion
