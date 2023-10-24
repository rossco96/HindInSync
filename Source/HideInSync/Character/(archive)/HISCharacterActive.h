// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HideInSync/Character/HISCharacter.h"
#include "HISCharacterActive.generated.h"

//friend class AHISCharacterClone;

UCLASS()
class HIDEINSYNC_API AHISCharacterActive : public AHISCharacter
{
	GENERATED_BODY()

public:
	AHISCharacterActive();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	void FindClone();

	AHISCharacterClone* PlayerClone;
	
	UPROPERTY(ReplicatedUsing = OnRep_FoundClone)						// [TODO] Do we need it replicated..?
	class AHISCharacterClone* FoundClone;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		void OnRep_FoundClone(AHISCharacterClone* LastClone);

public:
	void SetPlayerClone(AHISCharacterClone* Clone);
	void SetFoundClone(AHISCharacterClone* Clone);

};
