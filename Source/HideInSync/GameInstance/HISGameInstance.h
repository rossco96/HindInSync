// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HideInSync/Gameplay/GameplayEnums.h"
#include "HISGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API UHISGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FString GetGameModePath();
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetGameMode(FString Mode) { GameMode = Mode; UE_LOG(LogActor, Warning, TEXT(">>>>> GameMode SET (%s)"), *Mode); }

	UFUNCTION(BlueprintCallable)
	FString GetLevelPath();
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetLevelName(FString Level) { LevelName = Level; UE_LOG(LogActor, Warning, TEXT(">>>>> LevelName SET (%s)"), *Level); }

	FORCEINLINE int16 GetTimeLimit() { return TimeLimit; }
	FORCEINLINE int16 GetRespawnTime() { return RespawnTime; }
	
private:
	UPROPERTY(EditAnywhere)
	FString GameMode;							//EGameMode GameMode;
	UPROPERTY(EditAnywhere)
	FString LevelName;							//ELevelName LevelName;
	UPROPERTY(EditAnywhere)
	int16 TimeLimit;
	UPROPERTY(EditAnywhere)
	int16 RespawnTime;
};
