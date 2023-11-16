// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacterHider.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HideInSync/GameMode/HISGameMode.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
AHISCharacterHider::AHISCharacterHider()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}


void AHISCharacterHider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Hide", IE_Pressed, this, &AHISCharacterHider::HideClone);
}
#pragma endregion


#pragma region Input
void AHISCharacterHider::HideClone()
{
	if (HasAuthority())
	{
		FVector CloneLocation = GetTransform().GetLocation();
		FRotator CloneRotation = GetTransform().GetRotation().Rotator();
		AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
		HISGameMode->SetCloneHidingLocation(PlayerController, CloneLocation, CloneRotation);
	}
	else
	{
		ServerHideButtonPressed();
	}



	// [TODO][12.09.2023]
	//		o Get GameMode
	//		o Pass current position as hiding place
	//		o (CREATE NEW TYPE OF ACTOR - NOT CONTROLLED, CHARACTER SHELL, CONTAINS DATA OF LINKED PLAYER)
	//		o Show owner of THIS either a static bird's eye view of the map, or a free cam (unable to see other clones, obviously)
	//		o Wait until either all players hidden or hide timeout reached
	//		o GameMode then responsible for:
	//			- Spawn CLONE at the location each player chose
	//			- Respawn each player at original (randomly chosen) location, this time with ACTIVE character
	//			- Countdown until level start, and then start the game

	return;



	// [TODO]
	//	o Disable all input
	//	o Play some kind of disappearing animation
	//	o After e.g. 1 second, show a new camera
	//		(either a static birds-eye-view of the map, or a free-cam which cannot see other Hiders/clones)
	// -----
	//	o Also! Need HISCharacterHider to have ForceHideClone() method, 
	//		which can be called from HISGameMode (if timer runs out and player not hidden)



	//UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::HideClone]"));
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("HIDE CLONE!")));
	}
	// [TODO] Only need to set bOnlyRelevantToOwner to false?
	// Can have CollisionObjectType as ECC_WorldDynamic from start
	// ... And then what do about CollisionResponse Blocking?

	// [TODO] However! Want this executed by the GLM.
	// (Depending on the game state! If we're re-hiding mid-level, if settings permit, then want to execute it here... Right? Should still run via GLM? With timeout??)


	// [TODO] Spawn HISCharacterActive from EXISTING (also [TODO]) PlayerStart position
	// Same rules as above, spawning via GLM (etc.)


	// [TODO][IMPORTANT] Need all this interaction to be done within InLevelGameMode!!! 
	// i.e. Make call to Spawn the ActiveCharacter in there, and transfer ownership in there
}
#pragma endregion


#pragma region Server Functions
void AHISCharacterHider::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// This is only called on the server - no need to check HasAuthority()

	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::PossessedBy] PossessedBy -- Hider (child)"));

	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (HISGameMode)
	{
		if (PlayerController)
		{
			int ControllerPlayerId = PlayerController->GetPlayerId();
			//FVector Location = PlayerController->GetCurrentLocation();
			//FRotator Rotation = PlayerController->GetCurrentRotation();
			FVector Location = GetTransform().GetLocation();
			FRotator Rotation = GetTransform().GetRotation().Rotator();
			HISGameMode->SetSpawnLocation(ControllerPlayerId, Location, Rotation);
		}
	}
}


void AHISCharacterHider::ServerHideButtonPressed_Implementation()
{
	FVector CloneLocation = GetTransform().GetLocation();
	FRotator CloneRotation = GetTransform().GetRotation().Rotator();
	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	HISGameMode->SetCloneHidingLocation(PlayerController, CloneLocation, CloneRotation);
}
#pragma endregion
