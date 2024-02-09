// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"


/**
 * 
 */
UCLASS()
class HIDEINSYNC_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	// PostLogin is the first place where it's safe to access the player that just joined the game
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	class ALobbyPlayerController* HostController;

	UFUNCTION(BlueprintCallable)
	void StartGame(FString LevelPath, FString GameModePath);
};
