// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacterHider.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HideInSync/GameMode/HISGameMode.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
AHISCharacterHider::AHISCharacterHider()
{
	// Can delete the below, since inheriting from HISCharacter?
	//PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Don't think I need this below, as class inherits ACharacter (not APawn)
	// Would only be used for the AreaSphere code... Test it, I guess
	//bReplicates = true;

	// This is also not needed. Apparently. Again, think ACharacter already specifies? Somehow??
	//SetRootComponent(GetMesh());
}


void AHISCharacterHider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Hide", IE_Pressed, this, &AHISCharacterHider::HideClone);
}


void AHISCharacterHider::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Does anything need doing in here? Previous things have since been refactored out!
	}

	// Disable input in here?
	// Do we have a PlayerController assigned yet?
	// Do we need to poll for it in Tick??

	//bDisableInput = true;					// Need disabled in parent, after all
}


void AHISCharacterHider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion


#pragma region Input
void AHISCharacterHider::HideClone()
{
	if (HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::HideClone] HAS AUTH (i.e. we are the server)"));

		//GetMesh()->bOnlyOwnerSee = false;

		// [TODO][IMPORTANT] Change this! Set bOnlyOwnerSee (in BP itself) instead!
		// ... Will eventually be deleting this anyway since we're destroying and replacing with a clone
		//bOnlyRelevantToOwner = false;

		FVector CloneLocation = GetTransform().GetLocation();
		FRotator CloneRotation = GetTransform().GetRotation().Rotator();
		
		AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
		//HISGameMode->SetCloneHidingLocation(this, CloneLocation, CloneRotation);
		AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(GetController());	// [TODO] Store this cast, if using often?
		HISGameMode->SetCloneHidingLocation(HISPlayerController, CloneLocation, CloneRotation);
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

		
	// RPC THIS!!!
	//bOnlyRelevantToOwner = false;														// <<< Not doing like that anymore?


	// [TODO] Spawn HISCharacterActive from EXISTING (also [TODO]) PlayerStart position
	// Same rules as above, spawning via GLM (etc.)


	// [TODO][IMPORTANT] Need all this interaction to be done within InLevelGameMode!!! 
	// i.e. Make call to Spawn the ActiveCharacter in there, and transfer ownership in there
}
#pragma endregion


#pragma region Server Functions
//*
void AHISCharacterHider::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// This is only called on the server - no need to check HasAuthority()

	AHISPlayerController* HISController = Cast<AHISPlayerController>(NewController);
	if (HISController == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::PossessedBy] HISController == nullptr >>>>> HELP!!!!!"));
		return;
	}

	/*
	if (HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::PossessedBy] DisableInput - SERVER"));
		ClientDisableInput(HISController);
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::PossessedBy] disable_input_client"));
		ServerDisableInput(HISController);
	}
	//*/

	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (HISGameMode)
	{
		//HISGameMode->SetSpawnLocation(HISController->GetPlayerId(), GetTransform().GetLocation(), GetTransform().GetRotation().Rotator());
		int ControllerPlayerId = HISController->GetPlayerId();
		
		// [TODO][IMPORTANT] Check if class member PlayerId set in here already? Rename the above to ControllerPlayerId for now and use that
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::PossessedBy] (this)PlayerId = %i // ControllerPlayerId = %i"), PlayerId, ControllerPlayerId);
		
		FVector Location = HISController->GetCurrentLocation();
		FRotator Rotation = HISController->GetCurrentRotation();
		HISGameMode->SetSpawnLocation(ControllerPlayerId, Location, Rotation);
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::PossessedBy] s u c c e s s"));
	}
}
//*/


void AHISCharacterHider::ServerHideButtonPressed_Implementation()
{
	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterHider::ServerHideButtonPressed] (no auth)"));

	//GetMesh()->bOnlyOwnerSee = false;

	FVector CloneLocation = GetTransform().GetLocation();
	FRotator CloneRotation = GetTransform().GetRotation().Rotator();

	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	//HISGameMode->SetCloneHidingLocation(this, CloneLocation, CloneRotation);
	AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(GetController());	// [TODO] Store this cast, if using often?
	HISGameMode->SetCloneHidingLocation(HISPlayerController, CloneLocation, CloneRotation);
}


void AHISCharacterHider::ServerDisableInput_Implementation(AHISPlayerController* HISController)
{
	DisableInput(HISController);
}

void AHISCharacterHider::ClientDisableInput_Implementation(AHISPlayerController* HISController)
{
	DisableInput(HISController);
}
#pragma endregion
