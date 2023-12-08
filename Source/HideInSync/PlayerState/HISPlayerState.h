// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HISPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API AHISPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);								// SetScore() already exists as a function
private:
	class AHISCharacter* HISCharacter;
	class AHISPlayerController* HISPlayerController;
};
