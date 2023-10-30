// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HISHUD.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API AHISHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	class UCharacterOverlay* CharacterOverlay;
protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();
};
