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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHISCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHISCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AHISCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AHISCharacter::LookUp);
}


void AHISCharacter::BeginPlay()
{
	Super::BeginPlay();
}


void AHISCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AHISCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHISCharacter, PlayerId);
}
#pragma endregion


#pragma region Input
void AHISCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}


void AHISCharacter::MoveRight(float Value)
{
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
#pragma endregion
