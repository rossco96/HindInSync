// Fill out your copyright notice in the Description page of Project Settings.


#include "HISPlayerController.h"
#include "GameFramework/Character.h"
#include "HideInSync/HUD/Levels/HISHUD.h"


// vvv Testing only? Or actually need it now? vvv
void AHISPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// DEBUG BELOW
	FString IgnoredString = IsMoveInputIgnored() ? TEXT("inputignored") : TEXT("NOTignored");
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::BeginPlay] %s"), *IgnoredString);
	// DEBUG ABOVE

	// Disable input on spawning in:
	//SetIgnorePlayerInput(true);
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
void AHISPlayerController::SetIgnorePlayerInput(bool IgnoreInput)
{
	if (HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetIgnorePlayerControl] HasAuthority()"));
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetIgnorePlayerControl] NO AUTH"));
	}

	ClientIgnoreMoveInput(IgnoreInput);
	ClientIgnoreLookInput(IgnoreInput);

	return;

	// [TODO] Check HasAuthority? May need this as an RPC!
	if (HasAuthority())
	{
		bool MoveInputIgnoredBEF = IsMoveInputIgnored();
		FString IgnoredStringBEF = MoveInputIgnoredBEF ? TEXT("inputignored") : TEXT("NOTignored");
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetIgnorePlayerControl][SERVER] bef - %s"), *IgnoredStringBEF);
		//DisableInput(this);							// [TODO] THIS DOES NOT LOOK CORRECT? Should we be calling this from HISGameMode itself?

		SetIgnoreMoveInput(IgnoreInput);				// ... Or is this the right way of going about it?
		SetIgnoreLookInput(IgnoreInput);				// Do we want to be able to look around whilst still, or absolutely no movement?
		//SetIgnorePlayerInput(IgnoreInput);			// [TODO] Test this please! Assume it'll stop jumping... Unsure if we need this as well as or instead of the two above

		bool MoveInputIgnoredAFT = IsMoveInputIgnored();
		FString IgnoredStringAFT = MoveInputIgnoredAFT ? TEXT("inputignored") : TEXT("NOTignored");
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetIgnorePlayerControl][SERVER] aft - %s"), *IgnoredStringAFT);
	}
	else
	{
		//ClientSetIgnorePlayerInput(IgnoreInput);
		bool MoveInputIgnoredBEF = IsMoveInputIgnored();
		FString IgnoredStringBEF = MoveInputIgnoredBEF ? TEXT("inputignored") : TEXT("NOTignored");
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetIgnorePlayerControl][CLIENT] bef - %s"), *IgnoredStringBEF);
		//DisableInput(this);

		ClientIgnoreMoveInput(IgnoreInput);
		ClientIgnoreLookInput(IgnoreInput);

		bool MoveInputIgnoredAFT = IsMoveInputIgnored();
		FString IgnoredStringAFT = MoveInputIgnoredAFT ? TEXT("inputignored") : TEXT("NOTignored");
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::SetIgnorePlayerControl][CLIENT] aft - %s"), *IgnoredStringAFT);
	}
}
#pragma endregion


#pragma region Server RPC

#pragma endregion


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
		else
		{
			// THIS WILL GET HIT! Just leave rhis check in, no harm no foul?
			UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::UpdateTimer] HISHUD->CharacterOverlay == nullptr"));
		}
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::UpdateTimer] HISHUD == nullptr ... major issue! (break)"));
	}
}


void AHISPlayerController::ClientSetIgnorePlayerInput_Implementation(bool IgnoreInput)
{
	if (HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ClientSetIgnorePlayerInput_Implementation] HasAuthority()"));
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ClientSetIgnorePlayerInput_Implementation] NO AUTH"));
	}

	ClientIgnoreMoveInput(IgnoreInput);
	ClientIgnoreLookInput(IgnoreInput);

	return;



	bool MoveInputIgnoredBEF = IsMoveInputIgnored();
	FString IgnoredStringBEF = MoveInputIgnoredBEF ? TEXT("inputignored") : TEXT("NOTignored");
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ServerSetIgnorePlayerInput_Implementation][cli] bef - %s"), *IgnoredStringBEF);
	//DisableInput(this);							// [TODO] THIS DOES NOT LOOK CORRECT? Should we be calling this from HISGameMode itself?

	SetIgnoreMoveInput(IgnoreInput);				// ... Or is this the right way of going about it?
	SetIgnoreLookInput(IgnoreInput);

	bool MoveInputIgnoredAFT = IsMoveInputIgnored();
	FString IgnoredStringAFT = MoveInputIgnoredAFT ? TEXT("inputignored") : TEXT("NOTignored");
	UE_LOG(LogActor, Warning, TEXT("[AHISPlayerController::ServerSetIgnorePlayerInput_Implementation][cli] aft - %s"), *IgnoredStringAFT);
}
#pragma endregion





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
