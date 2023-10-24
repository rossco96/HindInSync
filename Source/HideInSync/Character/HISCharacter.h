// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HISCharacter.generated.h"

UCLASS()
class HIDEINSYNC_API AHISCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHISCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	UPROPERTY(Replicated)
	int PlayerId;

	bool bIsFound = false;									// We using this?

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	//int PlayerId;

public:	
	FORCEINLINE void SetPlayerId(int Id) { PlayerId = Id; }
	FORCEINLINE int GetPlayerId() { return PlayerId; }

	FORCEINLINE bool GetIsFound() const { return bIsFound; }
};
