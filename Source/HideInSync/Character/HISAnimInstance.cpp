// Fill out your copyright notice in the Description page of Project Settings.


#include "HISAnimInstance.h"
#include "HISCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UHISAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	HISCharacter = Cast<AHISCharacter>(TryGetPawnOwner());
}

void UHISAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (HISCharacter == nullptr)
	{
		HISCharacter = Cast<AHISCharacter>(TryGetPawnOwner());
		return;
	}

	FVector Velocity = HISCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	bIsInAir = HISCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = (HISCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f);
}
