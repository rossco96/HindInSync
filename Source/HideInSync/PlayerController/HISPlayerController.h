// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HideInSync/Gameplay/GameplayEnums.h"
#include "HISPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API AHISPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetPlayerId(int Id) { PlayerId = Id; }
	FORCEINLINE int GetPlayerId() { return PlayerId; }

	// IMPLEMENT PROPERLY!

	/*
	void SetSpawnPosition(FVector Location, FRotator Rotation);
	UFUNCTION(Server, Reliable)
	void ServerSetSpawnPosition(FVector Location, FRotator Rotation);
	//*/

	void SetHidePosition(FVector Location, FRotator Rotation);
	UFUNCTION(Server, Reliable)
	void ServerSetHidePosition(FVector Location, FRotator Rotation);

	// [NOTE] Only relevant for pre-game hiding! Will always use current player position for re-hiding
	bool bHidePositionSet = false;

	//void SetHUDScore(float Score);

	void SetHUDAnnoucementTimerText(float CountdownTime);
	void SetHUDTimerLabel(ETimerLabelState Label);
	void SetHUDGameTimerText(float CountdownTime);
	//void SetFoundTextVisible(bool bIsVisible);
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateHUDScores(int SlotNumber, int NewScore);
	
	// Above HUD set timer functions are from the course, below are mine...

	//UFUNCTION(Client, Reliable)
	//void ClientUpdateHUDTimerLabel(ETimerLabelState Label);
	//UFUNCTION(Client, Reliable)
	//void ClientUpdateHUDTimer(int Seconds);
	//UFUNCTION(Client, Reliable)
	//void ClientSetFoundTextVisible(bool bIsVisible);
	//UFUNCTION(Client, Reliable)
	//void ClientUpdateHUDScores(int SlotNumber, int NewScore);

	UPROPERTY(Replicated)
	bool bAllPlayersLoadedIn = false;							// Set from HISGameMode by server (then replicated to clients)
	UPROPERTY(Replicated)
	int TotalNumberOfPlayers = -1;

	// Synced with server world clock
	virtual float GetServerTime();
	// Sync with server clock as soon as possible
	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();

	void OnRespawnStateSet(ERespawnState State);
	void HandleRespawnStateChanged();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void PollInit();

	void SetHUDTime();

	class UHISGameInstance* HISGameInstance;

	float LevelStartingTime = 0.0f;			// The time the level was started by the server (according to HISGameMode)
	float GameTimerStartingTime = 0.0f;		// The time the game timer (which the players dictate a time limit for) is started
	float PreGameWaitTime = 0.0f;			// The countdown of everyone joining in (before the players physically spawn in)
	
	uint32 GameTimeInt = 0;					// Used to track a change in full seconds, so we're not updating the HUD timer every Tick
	int32 GameTimeLimit = 0;				// The time limit for the game, as chosen by the player
	int32 HideTimeLimit = 0;				// The hiding time limit for the game, as chosen by the player
	int32 RespawnWaitTime = 0;				// The wait for respawning, CURRENTLY as chosen by the player

	bool bIsGameTimeEndless = false;
	bool bIsHideTimeEndless = false;
	bool bNoRespawnTime = false;

	bool bGameTimerStarted = false;

	float SoloTimerStartTime = 0.0f;		// The (server) time the countdown started
	float SoloTimerLimit = 0.0f;			// The amount of time to count (e.g. 30s to Hide, or 3s wait for respawn movement)

	// Requests the current server time, passing in the client's time of request
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	// Reports the current server time to the client, in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceiveClientRequest);

	void CheckTimeSync(float DeltaTime);

	void CheckSoloTimer();

	// Difference between client and server time
	float ClientServerDelta = 0.0f;
	// How many seconds to re-sync the client server delta (e.g. in case of drift)
	UPROPERTY(EditAnywhere)
	float TimeSyncFrequency = 5.0f;
	// Time elapsed since previous time sync
	float TimeSyncRunningTime = 0.0f;

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	// Okay sending this much data, since only executing once!
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName State, float WaitTime, float StartingTime, float GameTime, float HideTime, float RespawnTime);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISCharacterHider> HiderClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISCharacterSeeker> SeekerClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHISClone> CloneClass;

	UPROPERTY(Replicated)
	int PlayerId;

	class AHISHUD* HISHUD;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	void SpawnCharacter(TSubclassOf<class AHISCharacter> CharacterType, bool DisableInput);	
	void SpawnClone();

	bool bInitialiseCharacterOverlay = false;				// bool for if we need to initialise the character overlay with any cached values
	bool bHUDScoresInitialised = false;						// bool for if the HUD scores has yet been initialised

	float HUDScore = 0.0f;
	float HUDCountdownTime = 0.0f;
	float HUDAnnouncementTime = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY(ReplicatedUsing = OnRep_RespawnState)
	ERespawnState RespawnState = ERespawnState::NONE;
	UFUNCTION()
	void OnRep_RespawnState();
	
	FVector SpawnLocation;
	FRotator SpawnRotation;
	FVector HideLocation;
	FRotator HideRotation;

	//bool bSpawnPositionSet = false;						// Shouldn't need this..?
};
