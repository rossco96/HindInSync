// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacterSeeker.h"
#include "Camera/CameraComponent.h"
#include "HISClone.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "HideInSync/GameMode/HISGameMode.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
AHISCharacterSeeker::AHISCharacterSeeker()
{
	POVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	POVCamera->SetupAttachment(GetMesh());
	POVCamera->bUsePawnControlRotation = true;
}


void AHISCharacterSeeker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Find", IE_Pressed, this, &AHISCharacterSeeker::FindClone);
}
#pragma endregion


#pragma region Input
void AHISCharacterSeeker::FindClone()
{
	if (FoundClone == nullptr) return;

	if (HasAuthority())
	{
		AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
		if (HISGameMode)
		{
			HISGameMode->PlayerFound(FoundClone, HISPlayerController);
		}
	}
	else
	{
		ServerFindButtonPressed();
	}
}
#pragma endregion


void AHISCharacterSeeker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AHISCharacterSeeker, FoundClone, COND_OwnerOnly);
}


void AHISCharacterSeeker::OnRep_FoundClone(AHISClone* LastClone)
{
	if (HISPlayerController == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::SetFoundClone] HISPlayerController is nullptr -- MAJOR ERROR AT THIS POINT !!!!!"));
		return;
	}

	if (FoundClone != nullptr && FoundClone->GetPlayerId() == HISPlayerController->GetPlayerId()) return;
	
	if (FoundClone)
	{
		FoundClone->ShowFoundWidget(true);
	}
	if (LastClone)
	{
		LastClone->ShowFoundWidget(false);
	}
}


void AHISCharacterSeeker::SetFoundClone(AHISClone* Clone)
{
	// [TODO][Important]
	// Here:
	//	o If a player hides at their own respawn location, this function is called and HISPlayerController is nullptr at this point
	// Some other bit of code:
	//	o If a player hides at another player's respawn location, the outline will not show up (they will have to move away and come back to find them)

	// Below is not ideal... See note above
	if (HISPlayerController == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::SetFoundClone] HISPlayerController is nullptr"));
		return;
	}

	if (Clone != nullptr && Clone->GetPlayerId() == HISPlayerController->GetPlayerId()) return;

	if (IsLocallyControlled() && FoundClone)
	{
		FoundClone->ShowFoundWidget(false);
	}

	FoundClone = Clone;
	if (IsLocallyControlled() && FoundClone)
	{
		FoundClone->ShowFoundWidget(true);
	}
}


#pragma region Server Functions
void AHISCharacterSeeker::ServerFindButtonPressed_Implementation()
{
	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (HISGameMode)
	{
		HISGameMode->PlayerFound(FoundClone, HISPlayerController);
	}
}
#pragma endregion
