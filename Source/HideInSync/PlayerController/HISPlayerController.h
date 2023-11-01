// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HISPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API AHISPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetPlayerId(int Id) { PlayerId = Id; }
	FORCEINLINE int GetPlayerId() { return PlayerId; }

	//FORCEINLINE FVector GetCurrentLocation() { return GetCharacter()->GetTransform().GetLocation(); }
	//FORCEINLINE FRotator GetCurrentRotation() { return GetActorRotation().Rotator(); }
	FVector GetCurrentLocation();
	FRotator GetCurrentRotation();

	void SetPlayerControl(bool IgnoreInput);
	void FreezePlayerControl();										// delete
	void ResumePlayerControl();										// delete

	UFUNCTION(Client, Reliable)										// Assume we need reliable, even if only update once every second?
	void ClientRPCUpdateHUDTimer(float ElapsedSeconds);				// Need this here since server cannot access player HUDs?

	virtual void BeginPlay() override;								// [TODO][DELETE] Testing only!

private:
	UPROPERTY(VisibleAnywhere)										// [TODO][DELETE] Testing only!
	int PlayerId;

	/*
	UFUNCTION(Server, Reliable)
	void ServerFreezePlayerControl();
	UFUNCTION(Server, Reliable)
	void ServerResumePlayerControl();
	//*/
};
