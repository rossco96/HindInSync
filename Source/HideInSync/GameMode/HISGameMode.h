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
	virtual void Tick(float DeltaSeconds) override;

	// PostLogin is the first place where it's safe to access the player that just joined the game
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;

	// [TODO]
	// WHY IS IT VIRTUAL?
	virtual void RequestRespawn();
	
	TMap<int, PlayerGameData> PlayersData;

	// [TODO] Delete - trigger a function at the end of the camera zoom out animation
	// --> Currently used in children (HISGameMode_FK) and NOT the base
	const float TEST_RespawnDelay = 3.0f;

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

	void UpdateJointWaitTimer();
	void UpdatePlayerHUDCountdown(int PlayerId);
	void UpdatePlayerHUDGameTimer(int PlayerId);
	void CheckGameTimer();
	bool bIsGameTimeEndless = false;
	bool bIsHideTimeEndless = false;
	bool bNoRespawnTime = true;
	
	// [TEST] Do not do like this!!
	// Need to consider:
	//	o How to get expected number of players
	//	o Then if anyone drops before joining
	//	o Then if anyone drops after loading into the level
	const int TEST_NumberOfPlayers = 3;

	// [TODO] Hack, not ideal, 'max' below returns a negative number??
	//		std::numeric_limits<float>::max()
	// (below is ~317 years, for anyone interested)
	const float MaxGameTime = 9999999999.9f;

	const float GameStartWaitTime = 5.0f;				// Used for both game start ONLY

	//const float TEST_HideTimeLimit = 8.0f;			// [TODO] DO NOT DO THIS! Want it customisable! DELETE!
	//const float RespawnTimeLimit = 3.0f;				// Consider including this (i.e. allow for a different starting hide time limit as to the one mid-game)

	int JointWaitTimeRemaining = 0;
	int CurrentGameTime = 0;

	void LevelLoadCountdownFinished();
	void JointHideTimerFinished();
	void JointSeekTimerFinished();
	void IndividualHiderRespawnWaitFinished();
	void IndividualHideTimerFinished();
	void IndividualSeekerRespawnWaitFinished();

	int GetPlayerIdByRespawnState(ERespawnState RespawnState);

	void SetCloneHidingLocationPreGame();
	void SetCloneHidingLocationIndividual(int PlayerId);
	void HideClone(int PlayerId);

	//void RestartPlayerAtPlayerStart();				// Consider different types of where and how to spawn the player(s)

public:
	void SetSpawnLocation(int PlayerId, FVector SpawnLocation, FRotator SpawnRotation);
	void SetCloneHidingLocation(class AHISPlayerController* HiderController, FVector Location, FRotator Rotation);

	// [TODO][Important] May need to reconsider signature of this function?
	virtual void PlayerFound(class AHISClone* FoundClone, class AHISPlayerController* SeekerController);
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
