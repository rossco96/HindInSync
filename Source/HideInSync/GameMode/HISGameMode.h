// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HideInSync/Gameplay/Data/PlayerGameData.h"
#include "HideInSync/Gameplay/GameplayEnums.h"
#include "HISGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API AHISGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AHISGameMode();
	virtual void Tick(float DeltaSeconds) override;

	// PostLogin is the first place where it's safe to access the player that just joined the game
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void CheckAllClonesHidden();
	void SpawnHiddenClone(AHISPlayerController* HISPlayerController);

	void SetSpawnLocation(int PlayerId, FVector SpawnLocation, FRotator SpawnRotation);

	// [TODO][Important] May need to reconsider signature of this function?
	virtual void PlayerFound(class AHISClone* FoundClone, class AHISPlayerController* SeekerController);

	UPROPERTY(EditDefaultsOnly)
	float GameStartWaitTime = 10.0f;				// Used for game start ONLY
	
	float LevelStartingTime = 0.0f;

protected:
	virtual void BeginPlay() override;
	
	TMap<int, PlayerGameData> PlayersData;

	// [TODO] Delete - trigger a function at the end of the camera zoom out animation
	// --> Currently used in children (HISGameMode_FK) and NOT the base
	const float TEST_RespawnDelay = 3.0f;

	virtual void OnMatchStateSet() override;

private:
	// [TODO] Think about how we're getting the cosmetics of each player to then dress the clone appropriately
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISCharacterHider> HiderClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISCharacterSeeker> SeekerClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISClone> CloneClass;

	class UHISGameInstance* HISGameInstance;
	int32 GameTimeLimit;
	int32 HideTimeLimit;
	int32 RespawnWaitTime;

	TMap<int, TMap<int, int>> GameScore;				// [IMPORTANT][Q] Do we want this here? Rather than on each individual PlayerGameData?

	FTimerHandle JointWaitTimer;
	FTimerHandle GameTimer;

	bool bHasInitialCountdownFinished = false;			// This feels a bit hacky..
	bool bIsCountingDownFirstSeek = false;				// .. And adding this feels even worse
	bool bHasGameStarted = false;

	bool bIsGameTimeEndless = false;
	bool bIsHideTimeEndless = false;
	bool bNoRespawnTime = true;
	
	// [TEST] Do not do like this!!
	// Need to consider:
	//	o How to get expected number of players
	//	o Then if anyone drops before joining
	//	o Then if anyone drops after loading into the level
	const int TEST_NumberOfPlayers = 3;

	const float MaxGameTime = INT32_MAX;

	float CurrentGameStartCountdown = 0.0f;

	int JointWaitTimeRemaining = 0;
	int CurrentGameTime = 0;

	int CurrentRespawnOrderId = 0;

	int GetPlayerIdByRespawnState(ERespawnState RespawnState);

	//void RestartPlayerAtPlayerStart();				// Consider different types of where and how to spawn the player(s)
};


// [TODO]
// Consider not doing the below. NOT CURRENTLY USED!
// Don't use the player starts as is built in,
// instead create a list of all start points
// (List<FVector> ... or whatever the notation is in C++)
// and then spawn in each player manually,
// removing the FVector from the list as you place each player.
// 
// This could still be useful!
// Could use this to also rule out player spawns within X range of a player, e.g.
// 
// x x x x
// x x x x
// x x x x
// 
//   |
//   v
// 
// o o x x
// * o x x
// o o x x

/*
USTRUCT()						// Is this macro essential?
struct FStartPointBool
{
	GENERATED_BODY()
public:
	FVector StartPosition;
	bool bIsUsed;
};
//*/
