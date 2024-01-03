// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreTextWidget.h"
#include "Components/TextBlock.h"

void UScoreTextWidget::UpdateScore(int NewScore)
{
	// Possibly need to null-check ScoreText ??? But then if null how to make sure it gets set...
	FString ScoreString = FString::FromInt(NewScore);
	ScoreText->SetText(FText::FromString(ScoreString));
}

void UScoreTextWidget::SetTextFromString(FString String)
{
	// Possibly need to null-check ScoreText ??? But then if null how to make sure it gets set...
	ScoreText->SetText(FText::FromString(String));
}
