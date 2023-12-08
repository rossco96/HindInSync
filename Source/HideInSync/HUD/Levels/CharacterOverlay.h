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

	void InitScorePanel(int NumberOfOtherPlayers);
	void AddToScore(int PlayerId, bool bAreWeTheSeeker);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreAmountText;

private:
	// [TODO] Move to public as well! Implement same way as ScoreText
	UPROPERTY(meta = (BindWidget))
	/*class*/ UTextBlock* GameTimer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FoundText;

	// [DELETE] Moved up to public
	//UPROPERTY(meta = (BindWidget))					// [TODO] Create UTextBlock and uncomment this!
	//UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* ScorePanel;

	UPROPERTY(EditAnywhere)
	class UWidget* TextBlockClass;
};
