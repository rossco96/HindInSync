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
	void UpdateTimer(int Seconds);
	
	void SetFoundTextVisible(bool bIsVisible);

	void AddTextToScorePanel(class UWidget* ScoreText, int PositionX, int PositionY);
	void UpdateScore(int SlotNumber, int NewScore);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreAmountText;

private:
	// [TODO] Move to public as well! Implement same way as ScoreText
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FoundText;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* ScorePanel;				// [TODO] May need to make public for access by HISHUD?
};
