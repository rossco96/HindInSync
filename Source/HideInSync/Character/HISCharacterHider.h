// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HISCharacter.h"
#include "HISCharacterHider.generated.h"

UCLASS()
class HIDEINSYNC_API AHISCharacterHider : public AHISCharacter
{
	GENERATED_BODY()

public:
	AHISCharacterHider();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// [TODO] Delete these two?
	FORCEINLINE FVector GetLocation() { return GetTransform().GetLocation(); }
	FORCEINLINE FRotator GetRotation() { return GetTransform().GetRotation().Rotator(); }

protected:
	virtual void PossessedBy(AController* NewController) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISClone> CloneClass;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	void HideClone();

	UFUNCTION(Server, Reliable)
	void ServerHideButtonPressed();
};
