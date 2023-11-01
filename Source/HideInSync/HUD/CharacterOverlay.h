// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateTimer(float ElapsedSeconds);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GameTimer;
};
