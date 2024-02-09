// Fill out your copyright notice in the Description page of Project Settings.


// [TODO][IMPORTANT] This is ALL only proof of concept! Needs tidying up and waaay less hard-coding!!!
// ... Must also be a better way of getting the enum value WITHOUT name of the enum than using Split()


#include "HISGameInstance.h"

//FString UHISGameInstance::GetGameModePathFromString(FString GameModeString)
FString UHISGameInstance::GetGameModePath()
{
	FString Path = "/Game/Blueprints/GameMode/BP_HISGameMode";

#pragma region v1.0
	// [TODO][IMORTANT] Convert string dropdowns to enums so can use switch statements ???
	/*
	if (GameMode == "Finders Keepers")
	{
		Path += "BP_HISGameMode_FK";
	}
	else
	{
		Path += "BP_HISGameMode";					// [TODO] I think this would cause an error???
	}
	//*/
	/*
	switch (GameMode)
	{
	case EGameMode::AON:
		break;
	case EGameMode::FK:
		Path += "";
		break;
	case EGameMode::LCS:
	case EGameMode::Classic:
	case EGameMode::Sardines:
	case EGameMode::none:
	default:
		break;
	}
	//*/
#pragma endregion

#pragma region v2.0
	// [TODO] Must be an easier way than this!!!
	/*
	FString FullGameModeEnum = UEnum::GetValueAsString(GameMode);
	FString EnumName;
	FString Mode;
	FullGameModeEnum.Split(TEXT("::"), &EnumName, &Mode);
	
	Path += "_" + Mode;
	//*/
#pragma endregion;

	Path += "_" + GameMode;

	return Path;
}


FString UHISGameInstance::GetLevelPath()
{
	FString Path = "/Game/Maps/";
	
#pragma region v1.0
	/*
	if (LevelName == ELevelName::TEST)
	{
		Path += "Test/HISMap";
	}
	else
	{
		// [TODO] Must be an easier way than this!!!
		FString FullLevelNameEnum = UEnum::GetValueAsString(LevelName);
		FString EnumName;
		FString Level;
		FullLevelNameEnum.Split(TEXT("::"), &EnumName, &Level);

		Path += "Levels/" + UEnum::GetValueAsString(LevelName);
	}
	//*/
#pragma endregion

	UE_LOG(LogActor, Warning, TEXT(">>>>> >>>>> >>>>> %s"), *LevelName);

	if (LevelName == "TEST")
	{
		UE_LOG(LogActor, Warning, TEXT(">>>>> >>>>> >>>>> a"));
		Path += "Test/HISMap";
	}
	else
	{
		UE_LOG(LogActor, Warning, TEXT(">>>>> >>>>> >>>>> b"));
		Path += "Levels/" + LevelName;
	}

	return Path;
}
