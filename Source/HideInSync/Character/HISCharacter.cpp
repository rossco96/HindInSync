// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
AHISCharacter::AHISCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// [TODO] Only here & false temporarily!
	// Do we want false with Hider (3rd person) and true with Seeker (1st person) ?
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}


void AHISCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHISCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHISCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AHISCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AHISCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHISCharacter::Jump);
}


void AHISCharacter::BeginPlay()
{
	Super::BeginPlay();

	bDisableInput = true;
}


/*
void AHISCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() == false) return;
	//if (IsLocalPlayerController() == false) return;

	if (bHasPlayerController == false)
	{
		PlayerController = Cast<AHISPlayerController>(GetController());
		if (PlayerController)
		{
			bHasPlayerController = true;
		}
	}
}
//*/


void AHISCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHISCharacter, bDisableInput);
}
#pragma endregion


#pragma region Input
void AHISCharacter::MoveForward(float Value)
{
	if (bDisableInput) return;
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AHISCharacter::MoveRight(float Value)
{
	if (bDisableInput) return;
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}


void AHISCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AHISCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}


void AHISCharacter::Jump()
{
	if (bDisableInput) return;
	Super::Jump();
}
#pragma endregion


void AHISCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// This is only called on the server - no need to check HasAuthority()

	PlayerController = Cast<AHISPlayerController>(NewController);
	ClientSetPlayerController(PlayerController);													// THIS FEELS WRONG? But not sure why else I'd be getting the error...
	UE_LOG(LogActor, Warning, TEXT("[AHISCharacter::PossessedBy] PossessedBy -- parent // We must be the Server in this function?"));
}


void AHISCharacter::ClientSetPlayerController_Implementation(AHISPlayerController* NewController)
{
	PlayerController = NewController;
	UE_LOG(LogActor, Warning, TEXT("[AHISCharacter::ClientSetPlayerController] ClientRPC."));
}
