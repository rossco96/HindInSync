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
	virtual void DrawHUD() override;						// Currently doing nothing, but included in the course

	void InitScorePanel(int NumberOfPlayers);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> CharacterOverlayClass;	// Does this need to be public?

	class UCharacterOverlay* CharacterOverlay;

protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();

private:
	UPROPERTY(EditAnywhere)
	class TSubclassOf<UUserWidget> ScoreTextWidgetClass;
};
