// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HISCharacter.h"
#include "HISCharacterHider.generated.h"

UCLASS()
class HIDEINSYNC_API AHISCharacterHider : public AHISCharacter
{
	GENERATED_BODY()

public:
	AHISCharacterHider();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	void HideClone();

	// [TODO] Don't need to store these? Can just pass to HISGameMode (or better, HISPlayerController) upon spawning?
	//FVector SpawnLocation;
	//FRotator SpawnRotation;

	UFUNCTION(Server, Reliable)
	void ServerHideButtonPressed();

	virtual void PossessedBy(AController* NewController);									// DELETE?

public:
	FORCEINLINE FVector GetLocation() { return GetTransform().GetLocation(); }
	FORCEINLINE FRotator GetRotation() { return GetTransform().GetRotation().Rotator(); }
};
