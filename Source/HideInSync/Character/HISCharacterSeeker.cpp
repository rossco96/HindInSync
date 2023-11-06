// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacterSeeker.h"
#include "HISClone.h"									// [TODO] Turn into a friend class, to get references of Active from Clone? Is that how it works?
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"							// [TODO] Do we need to have this here if it's also in the parent??
#include "HideInSync/GameMode/HISGameMode.h"
//#include "HideInSync/PlayerController/HISPlayerController.h"	// DELETE DELETE DELETE


#pragma region Unreal Functions
AHISCharacterSeeker::AHISCharacterSeeker()
{
	// Can delete the below, since inheriting from HISCharacter?
 	//PrimaryActorTick.bCanEverTick = true;

	POVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	POVCamera->SetupAttachment(GetMesh());
	POVCamera->bUsePawnControlRotation = true;
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//FollowCamera->bUsePawnControlRotation = false;

	//Controller->Possess(this);											// HELP! THIS CAUSES A CRASH!
}


void AHISCharacterSeeker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Find", IE_Pressed, this, &AHISCharacterSeeker::FindClone);
}


void AHISCharacterSeeker::BeginPlay()
{
	Super::BeginPlay();

	//bDisableInput = true;				// Need disabled in parent, after all
}


void AHISCharacterSeeker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion


#pragma region Input
void AHISCharacterSeeker::FindClone()
{
	if (HasAuthority())
	{
		//UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] HAS AUTH (i.e. we are the server)"));

		//UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] A"));
		if (FoundClone == nullptr)
		{
		//	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] X"));
			return;
		}

		//UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] B"));
		AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
		if (HISGameMode)
		{
		//	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] C"));
			//int CloneId = FoundClone->GetPlayerId();
			//AHISPlayerController* CloneHISController = FoundClone->GetHISPlayerController();
			AHISPlayerController* HISController = Cast<AHISPlayerController>(Controller);
			//HISController = (HISController == nullptr) ? Cast<AHISPlayerController>(Controller) : HISController;

			if (HISController)
			{
		//		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] D"));
				HISGameMode->PlayerFound(FoundClone, HISController);
			}
			else
			{
		//		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] Z"));
			}
		}
		else
		{
		//	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::FindClone] Y"));
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
	if (FoundClone != nullptr && FoundClone->GetPlayerId() == PlayerId) return;		// Is this the best way to do this?
	
	// DEBUG BELOW
	/*
	if (GetOwner() != nullptr && GetController() != nullptr)
	{
		FString OwnerString = GetOwner()->GetActorNameOrLabel();
		FString ControllerString = GetController()->GetActorNameOrLabel();
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::OnRep_FoundClone] OWNER = %s // CONTROLLER = %s"), *OwnerString, *ControllerString);
	}
	//*/
	
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
	if (Clone != nullptr && Clone->GetPlayerId() == PlayerId) return;				// Is this the best way to do this?

	// vvv vvv DEBUG vvv vvv
	/*
	if (GetOwner() != nullptr && GetController() != nullptr)
	{
		FString OwnerString = GetOwner()->GetActorNameOrLabel();
		FString ControllerString = GetController()->GetActorNameOrLabel();
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::SetFoundClone] OWNER = %s // CONTROLLER = %s"), *OwnerString, *ControllerString);
	}
	//*/
	// ^^^ ^^^ DEBUG ^^^ ^^^

	if (IsLocallyControlled() && FoundClone)
	{
		//UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::SetFoundClone] IsLocallyControlled(1)"));
		FoundClone->ShowFoundWidget(false);
	}

	FoundClone = Clone;
	if (IsLocallyControlled() && FoundClone)
	{
		//UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::SetFoundClone] IsLocallyControlled(2)"));
		FoundClone->ShowFoundWidget(true);
	}
}


#pragma region Server Functions
void AHISCharacterSeeker::ServerFindButtonPressed_Implementation()
{
	// Is nullptr check needed?
	if (FoundClone == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::ServerHideButtonPressed_Implementation] X"));
		return;
	}

	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (HISGameMode)
	{
		//int CloneId = FoundClone->GetPlayerId();
		//AHISPlayerController* CloneHISController = FoundClone->GetHISPlayerController();
		AHISPlayerController* HISController = Cast<AHISPlayerController>(Controller);
		//HISController = (HISController == nullptr) ? Cast<AHISPlayerController>(Controller) : HISController;

		if (HISController)
		{
			HISGameMode->PlayerFound(FoundClone, HISController);
		}
	}
}
#pragma endregion
