// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
//#include "HideInSync/Gameplay/WinConditions/WinCondition.h"
#include "HideInSync/Gameplay/Data/PlayerGameData.h"
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
	// [Q] Why do we NOT need CloneClass as protected? Or do we???

	void StartWaitTimer(float Duration, FSimpleDelegate::TMethodPtr<AHISGameMode> OnComplete);
	void StartWaitTimerWithInputEnabled(float Duration, FSimpleDelegate::TMethodPtr<AHISGameMode> OnComplete, class AHISPlayerController* HISController, bool InputEnabled);
	void StartGameTimer();

	// [TODO]
	// Think I can get away with this being protected?
	// Was previously public -- WHY IS IT VIRTUAL?
	virtual void RequestRespawn(int PlayerId);
	
	TMap<int, PlayerGameData> PlayersData;

private:
	// [TODO] Think about how we're getting the cosmetics of each player to then dress the clone appropriately
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISCharacterHider> HiderClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISCharacterSeeker> SeekerClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISClone> CloneClass;

	FTimerHandle WaitTimer;
	FTimerHandle GameTimer;

	FTimerHandle GameStartTimer;
	FTimerHandle HideTimer;
	FTimerHandle RespawnDelayTimer;
	FTimerHandle RespawnTimer;

	bool bHasGameStarted = false;
	void UpdatePlayerTimers();
	
	// [TEST] Do not do like this!!
	const int TEST_NumberOfPlayers = 3;

	// [TODO][IMPORTANT] 10.0f for testing only! Want 3.0f
	const float WaitCountdown = 10.0f;					// Used for both game start, and respawning ... any others?

	const float HideTimeLimit = 15.0f;					// [TODO] DO NOT DO THIS! Want it customisable! DELETE!
	const float RespawnDelay = 1.0f;					// [TODO] Delete this too - trigger a function at the end of the camera zoom out animation
	const float RespawnTimeLimit = 3.0f;				// Also customisable?

	void InitialCountdownFinished();
	void InitialHideTimerFinished();

	void HideClone(int PlayerId);

public:
	void SetSpawnLocation(int PlayerId, FVector SpawnLocation, FRotator SpawnRotation);
	void SetCloneHidingLocation(class AHISPlayerController* HiderController, FVector Location, FRotator Rotation);
	//void SetCloneHidingLocation(class AHISCharacterHider* Hider, FVector Location, FRotator Rotation);

	virtual void PlayerFound(class AHISClone* FoundClone, /*class AHISPlayerController* HiderController,*/ class AHISPlayerController* SeekerController);
	//virtual void RequestRespawn(class AHISCharacter* CurrentCharacter, class AHISCharacter* NextCharacter, class AHISPlayerController* HISPlayerController);
	
	// [TODO] Could turn this into one function, RequestRespawn, simply passing AHISCharacter? Or would it be easier to have separate???
	//virtual void RequestRespawnHider(class AHISCharacterHider* Hider, class AHISPlayerController* HISPlayerController);
	//virtual void RequestRespawnSeeker(class AHISCharacterSeeker* Seeker, class AHISPlayerController* HISPlayerController);
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
