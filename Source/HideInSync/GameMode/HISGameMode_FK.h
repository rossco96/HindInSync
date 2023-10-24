// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HISGameMode.h"
#include "GameFramework/GameMode.h"
#include "HISGameMode_FK.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API AHISGameMode_FK : public AHISGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerFound(class AHISClone* FoundClone, /*class AHISPlayerController* HiderController,*/ class AHISPlayerController* SeekerController) override;
};
