// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API UScoreTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateScore(int NewScore);
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreText;
};
