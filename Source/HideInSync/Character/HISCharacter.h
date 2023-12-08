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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(Replicated)
	bool bDisableInput = false;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Jump();

	class AHISPlayerController* HISPlayerController;
	class AHISPlayerState* HISPlayerState;

	bool bIsFound = false;										// We using this??

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// Poll for any relevant classes and initialise (on Tick)
	void PollInit();

	UFUNCTION(Client, Reliable)
	void ClientSetPlayerController(AHISPlayerController* NewController);

public:	
	FORCEINLINE bool GetIsFound() const { return bIsFound; }	// We using this?
};
