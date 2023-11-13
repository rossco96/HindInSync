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

	FORCEINLINE FVector GetCurrentLocation() { return GetCharacter()->GetTransform().GetLocation(); }
	FORCEINLINE FRotator GetCurrentRotation() { return GetCharacter()->GetTransform().GetRotation().Rotator(); }

	UFUNCTION(Client, Reliable)
	void ClientUpdateHUDTimer(int Seconds);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	int PlayerId;
};
