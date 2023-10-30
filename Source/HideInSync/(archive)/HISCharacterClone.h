// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HideInSync/Character/HISCharacter.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "HISCharacterClone.generated.h"

UCLASS()
class HIDEINSYNC_API AHISCharacterClone : public AHISCharacter
{
	GENERATED_BODY()

public:
	AHISCharacterClone();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	void HideClone();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHISCharacter> ActiveCharacterBP;

	FVector SpawnLocation;
	FRotator SpawnRotation;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* FoundWidget;

	UFUNCTION(Server, Reliable)
	void ServerHideButtonPressed();

	// If binding to a component's OnBeginOverlap, then NEED this signature:
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:
	AHISPlayerController* GetHISPlayerController();
	
	void ShowFoundWidget(bool bShowWidget);
	
	void FoundIgnore();
	UFUNCTION(NetMulticast, Reliable)
	void FoundReset();
	void FoundSpectate();
};
