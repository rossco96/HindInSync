// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HISCharacter.h"
#include "HISCharacterSeeker.generated.h"

//friend class AHISCharacterClone;

UCLASS()
class HIDEINSYNC_API AHISCharacterSeeker : public AHISCharacter
{
	GENERATED_BODY()

public:
	AHISCharacterSeeker();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// [NOTE][IMPORTANT] Gave up with 'Parent Socket' issue and have added manually through the BluePrint...
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* POVCamera;

	void FindClone();
	
	UPROPERTY(ReplicatedUsing = OnRep_FoundClone)						// [TODO] Do we need it replicated..?
	class AHISClone* FoundClone;

	UFUNCTION()
	void OnRep_FoundClone(AHISClone* LastClone);

	UFUNCTION(Server, Reliable)
	void ServerFindButtonPressed();

public:
	void SetFoundClone(AHISClone* Clone);
};
