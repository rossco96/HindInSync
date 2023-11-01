// Fill out your copyright notice in the Description page of Project Settings.


#include "HISPlayerController.h"
#include "GameFramework/Character.h"
#include "HideInSync/HUD/HISHUD.h"


// vvv Testing only! vvv
void AHISPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bool MoveInputIgnored = IsMoveInputIgnored();
	FString IgnoredString = MoveInputIgnored ? TEXT("inputignored") : TEXT("NOTignored");
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetPlayerControl] %s"), *IgnoredString);
}


#pragma region GETs
/*
FVector AHISPlayerController::GetStartPosition()
{
	return StartPosition;
}

FRotator AHISPlayerController::GetStartRotation()
{
	return StartRotation;
}
//*/

FVector AHISPlayerController::GetCurrentLocation()
{
	// DEBUG DEBUG DEBUG
	if (GetCharacter() == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::GetCurrentLocation] nullptr"));
		/*
		FOnPawnBeginPlay(
		{
			//UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::GetCurrentLocation] que?"));
			if (GetCharacter() != nullptr)
			{
				UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::GetCurrentLocation] NOT SO NULL POINTER NOW ARE YOU"));
				return GetCharacter()->GetTransform().GetLocation();
			}
		});
		//*/
		return FVector();														// Expect a crash if commenting out this line...
	}
	// DEBUG DEBUG DEBUG

	// [TODO][IMPORTANT] Is GetCharacter() the best/only way to do this???
	// Can't use GetActorLocation ("Hidden functions that don't make sense to use on this class")
	return GetCharacter()->GetTransform().GetLocation();
}

FRotator AHISPlayerController::GetCurrentRotation()
{
	if (GetCharacter() == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::GetCurrentRotation] nullptr"));
		return FRotator();
	}

	// [TODO][IMPORTANT] Is GetCharacter() the best/only way to do this???
	return GetCharacter()->GetTransform().GetRotation().Rotator();
}
#pragma endregion


#pragma region SETs
/*
void AHISPlayerController::SetStartLocation(FVector Position, FRotator Rotation)
{
	StartPosition = Position;
	StartRotation = Rotation;
}
//*/
#pragma endregion


#pragma region Player Control
void AHISPlayerController::SetPlayerControl(bool IgnoreInput)
{
	bool MoveInputIgnoredBEF = IsMoveInputIgnored();
	FString IgnoredStringBEF = MoveInputIgnoredBEF ? TEXT("inputignored") : TEXT("NOTignored");
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetPlayerControl] bef - %s"), *IgnoredStringBEF);
	//DisableInput(this);							// [TODO] THIS DOES NOT LOOK CORRECT? Should we be calling this from HISGameMode itself?

	SetIgnoreMoveInput(IgnoreInput);				// ... Or is this the right way of going about it?
	SetIgnoreLookInput(IgnoreInput);

	bool MoveInputIgnoredAFT = IsMoveInputIgnored();
	FString IgnoredStringAFT = MoveInputIgnoredAFT ? TEXT("inputignored") : TEXT("NOTignored");
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetPlayerControl] aft - %s"), *IgnoredStringAFT);
}


void AHISPlayerController::FreezePlayerControl()
{
	//if (HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::FreezePlayerControl] xxxxx"));
		//DisableInput(this);							// [TODO] THIS DOES NOT LOOK CORRECT? Should we be calling this from HISGameMode itself?
	
		SetIgnoreLookInput(true);						// ... Or is this the right way of going about it?
		SetIgnoreMoveInput(true);
	}
	//else
	//{
	//	ServerFreezePlayerControl();
	//}
}

void AHISPlayerController::ResumePlayerControl()
{
	//if (HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ResumePlayerControl] ooooo"));
		SetIgnoreLookInput(false);
		SetIgnoreMoveInput(false);
	}
	//else
	//{
	//	ServerResumePlayerControl();
	//}
}
#pragma endregion


void AHISPlayerController::ClientRPCUpdateHUDTimer_Implementation(float ElapsedSeconds)
{
	AHISHUD* HISHUD = Cast<AHISHUD>(GetHUD());						// [TODO][IMPORTANT] Should store this at the very start! Do not call Cast on Tick!
	if (HISHUD)
	{
		HISHUD->CharacterOverlay->UpdateTimer(ElapsedSeconds);
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::UpdateTimer] HISHUD == nullptr ... major issue! (break)"));
	}
}





// DO NOT NEED THE BELOW, BY THE LOOKS OF IT
// ... Try somethinng else

/*
#pragma region SERVER
void AHISPlayerController::ServerFreezePlayerControl_Implementation()
{
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SERVER_FreezePlayerControl] xxxxxserver"));
	//DisableInput(this);							// [TODO] THIS DOES NOT LOOK CORRECT? Should we be calling this from HISGameMode itself?

	SetIgnoreLookInput(true);						// ... Or is this the right way of going about it?
	SetIgnoreMoveInput(true);
}

void AHISPlayerController::ServerResumePlayerControl_Implementation()
{
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SERVER_ResumePlayerControl] oooooserver"));
	SetIgnoreLookInput(false);
	SetIgnoreMoveInput(false);
}
#pragma endregion
//*/
