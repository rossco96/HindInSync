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
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* POVCamera;

	void RaycastViewport();
	void FindClone();
	
	UPROPERTY(ReplicatedUsing = OnRep_FoundClone)
	class AHISClone* FoundClone;

	UFUNCTION()
	void OnRep_FoundClone(AHISClone* LastClone);

	UFUNCTION(Server, Reliable)
	void ServerFindButtonPressed();

	UPROPERTY(Replicated)
	int ClonesInRange = 0;
	bool bFoundCloneVisible = false;

public:
	void SetCloneInRange(AHISClone* Clone, bool bIsInRange);
};
