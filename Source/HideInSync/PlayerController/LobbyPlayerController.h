// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void HostSetHUDButtonStartGameEnabled(bool Enabled);
};
