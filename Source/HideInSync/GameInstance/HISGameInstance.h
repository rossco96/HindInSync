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
	FORCEINLINE FString GetGameModePath() { return GameModeFilePath + GameMode; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetGameMode(FString Mode) { GameMode = Mode; }

	// [TODO][TEST] Once TEST map is no longer being used, can compact GetLevelPath() so it's the same as GetGameModePath()
	// --> And be sure to amend LevelMapFilePath to include "Levels/"
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetLevelPath() { if (LevelName == "TEST") { return LevelMapFilePath + "Test/HISMap"; } else { return LevelMapFilePath + "Levels/" + LevelName; } }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetLevelName(FString Level) { LevelName = Level; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetGameTimeLimit() { return GameTimeLimit; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetGameTimeLimit(int32 Time) { GameTimeLimit = Time; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetHideTimeLimit() { return HideTimeLimit; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetHideTimeLimit(int32 Time) { HideTimeLimit = Time; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetRespawnTime() { return RespawnTime; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetRespawnTime(int32 Time) { RespawnTime = Time; }
	
private:
	FString GameMode;
	FString LevelName;
	int32 GameTimeLimit;
	int32 HideTimeLimit;
	int32 RespawnTime;

	const FString GameModeFilePath = "/Game/Blueprints/GameMode/BP_HISGameMode_";
	const FString LevelMapFilePath = "/Game/Maps/";
};
