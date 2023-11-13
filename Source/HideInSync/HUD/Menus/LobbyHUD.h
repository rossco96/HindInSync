// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> LobbyOverlayHostClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> LobbyOverlayClientClass;

	class ULobbyOverlayBase* LobbyOverlay;
private:
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientAddLobbyOverlay(bool IsHost);
};
