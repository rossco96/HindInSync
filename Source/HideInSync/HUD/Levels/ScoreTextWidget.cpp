// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreTextWidget.h"
#include "Components/TextBlock.h"

void UScoreTextWidget::UpdateScore(int NewScore)
{
	FString ScoreString = FString::FromInt(NewScore);
	ScoreText->SetText(FText::FromString(ScoreString));
}
