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
	//FORCEINLINE FRotator GetCurrentRotation() { return GetCharacter()->GetTransform().GetRotation().Rotator(); }
	
	// [TEMP] Below for debugging only!!! Delete these, and in .cpp file, and redo the above (unless it's needed...)
	FVector GetCurrentLocation();
	FRotator GetCurrentRotation();

	// ..... Potentially don't need GetCurrentLocation() or Rotation() at all??
	// Somewhat circular as(I think) only references are coming from HISCharacter classes..?
	// [TODO][IMPORTANT]
	// These functions *do* need to stay, but should not be used by HISCharacter (or children)
	// -- currently used by HISGameMode (PlayersData)

	UFUNCTION(Client, Reliable)
	void ClientUpdateHUDTimer(int Seconds);

	UFUNCTION(Client, Reliable)
	void ClientSetFoundTextVisible(bool bIsVisible);

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	int PlayerId;

	class AHISHUD* HISHUD;
	bool bHasHUD = false;
};
