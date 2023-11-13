// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HISCharacter.h"
#include "HISCharacterSeeker.generated.h"

UCLASS()
class HIDEINSYNC_API AHISCharacterSeeker : public AHISCharacter
{
	GENERATED_BODY()

public:
	AHISCharacterSeeker();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* POVCamera;

	void FindClone();
	
	UPROPERTY(ReplicatedUsing = OnRep_FoundClone)
	class AHISClone* FoundClone;

	UFUNCTION()
	void OnRep_FoundClone(AHISClone* LastClone);

	UFUNCTION(Server, Reliable)
	void ServerFindButtonPressed();

public:
	void SetFoundClone(AHISClone* Clone);
};
