// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacterActive.h"
#include "HISCharacterClone.h"									// [TODO] Turn into a friend class, to get references of Active from Clone? Is that how it works?
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "HideInSync/GameMode/HISGameMode.h"


#pragma region Unreal Functions
AHISCharacterActive::AHISCharacterActive()
{
	// Can probably delete the below, since inheriting from HISCharacter?
 	PrimaryActorTick.bCanEverTick = true;

	//Controller->Possess(this);											// HELP! THIS CAUSES A CRASH!
}


void AHISCharacterActive::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Find", IE_Pressed, this, &AHISCharacterActive::FindClone);
}


void AHISCharacterActive::BeginPlay()
{
	Super::BeginPlay();
}


void AHISCharacterActive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion


#pragma region Input
void AHISCharacterActive::FindClone()
{
	if (FoundClone == nullptr)
	{
		return;
	}

	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (HISGameMode)
	{
		AHISPlayerController* CloneHISController = FoundClone->GetHISPlayerController();
		AHISPlayerController* HISController = Cast<AHISPlayerController>(Controller);
		//HISController = (HISController == nullptr) ? Cast<AHISPlayerController>(Controller) : HISController;
		
		if (CloneHISController && HISController)
		{
			//HISGameMode->PlayerFound(FoundClone, CloneHISController, HISController);							// HISCharacterActive is getting deleted!
		}
	}
}
#pragma endregion


void AHISCharacterActive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AHISCharacterActive, FoundClone, COND_OwnerOnly);
}


void AHISCharacterActive::OnRep_FoundClone(AHISCharacterClone* LastClone)
{
	if (FoundClone)
	{
		FoundClone->ShowFoundWidget(true);
	}
	if (LastClone)
	{
		LastClone->ShowFoundWidget(false);
	}
}


void AHISCharacterActive::SetFoundClone(AHISCharacterClone* Clone)
{
	if (IsLocallyControlled())
	{
		if (FoundClone)
		{
			FoundClone->ShowFoundWidget(false);
		}
	}

	FoundClone = Clone;
	if (IsLocallyControlled())
	{
		if (FoundClone)
		{
			FoundClone->ShowFoundWidget(true);
		}
	}
}


void AHISCharacterActive::SetPlayerClone(AHISCharacterClone* Clone)
{
	PlayerClone = Clone;
}
