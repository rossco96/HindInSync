// Fill out your copyright notice in the Description page of Project Settings.


#include "HISGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "HideInSync/Character/HISCharacter.h"
#include "HideInSync/Character/HISCharacterHider.h"
#include "HideInSync/Character/HISCharacterSeeker.h"
#include "HideInSync/Character/HISClone.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


#pragma region UNREAL FUNCTIONS
void AHISGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AHISGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsJointWaitTimer)
	{
		UpdateJointWaitTimer();
		return;
	}

	CheckGameTimer();

	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		if (PlayersData[i].bIsHidden)
		{
			UpdatePlayerHUDGameTimer(i);
		}
		else
		{
			UpdatePlayerHUDCountdown(i);
		}
	}
}

void AHISGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	int32 PlayerId = NumberOfPlayers - 1;												// Id should be zero-based index
	
	AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(NewPlayer);
	HISPlayerController->SetPlayerId(PlayerId);
	PlayerGameData Data = PlayerGameData(HISPlayerController);

	AHISCharacterHider* HISHider = Cast<AHISCharacterHider>(HISPlayerController->GetCharacter());
	if (HISHider)
	{
		Data.SetSpawnPosition(HISHider->GetLocation(), HISHider->GetRotation());
	}
	PlayersData.Add(PlayerId, Data);

	if (NumberOfPlayers == TEST_NumberOfPlayers)
	{
		// START THE COUNTDOWN!
		GetWorldTimerManager().SetTimer(JointWaitTimer, this, &AHISGameMode::LevelLoadCountdownFinished, WaitCountdown);
	}
}
#pragma endregion


#pragma region (OnTimersComplete -- pregame)
// [TODO][IMPORTANT] Want to avoid doing the below!
// Want everyone to be loaded... I guess still wait for a couple of seconds,
// and then have the actual hide countdown begin
void AHISGameMode::LevelLoadCountdownFinished()
{
	// [TODO][Q] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers? What if people drop out!?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
		HISCharacter->bDisableInput = false;
	}

	GetWorldTimerManager().SetTimer(JointWaitTimer, this, &AHISGameMode::JointHideTimerFinished, TEST_HideTimeLimit);
}

void AHISGameMode::JointHideTimerFinished()
{
	// If NOT all players hidden by now, must force hide remaining players!
	
	// [TODO] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		// DISABLE INPUT -- Not required? Since immediately spawning a seeker (with input set to disabled from BeginPlay)
		AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
		HISCharacter->bDisableInput = true;
		HideClone(i);
		PlayersData[i].bIsHidden = true;
	}

	GetWorldTimerManager().SetTimer(JointWaitTimer, this, &AHISGameMode::JointSeekTimerFinished, WaitCountdown);
}

void AHISGameMode::JointSeekTimerFinished()
{
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		// ENABLE INPUT
		AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
		HISCharacter->bDisableInput = false;
	}

	bIsJointWaitTimer = false;													// Are these bools
	bHasGameStarted = true;														// two of the same?
	GetWorldTimerManager().SetTimer(GameTimer, MaxGameTime, false);
}
#pragma endregion


#pragma region (OnTimersComplete -- individual)
// [TODO][IMPORTANT] This will almost definitely be deleted -- want to have the respawn wait function triggered via FOUND animation!
void AHISGameMode::IndividualHiderRespawnWaitFinished()
{
	int PlayerId = GetPlayerIdByRespawnState(ERespawnState::RSE_RespawnHiderWait);
	AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[PlayerId].GetController()->GetPawn());
	HISCharacter->bDisableInput = false;
	FTimerHandle* WaitTimer = PlayersData[PlayerId].GetWaitTimer();
	GetWorldTimerManager().SetTimer(*WaitTimer, this, &AHISGameMode::IndividualHideTimerFinished, TEST_HideTimeLimit);
	PlayersData[PlayerId].RespawnState = ERespawnState::RSE_Hider;
}

void AHISGameMode::IndividualHideTimerFinished()
{
	// DISABLE INPUT -- Not required? Since immediately spawning a seeker (with input set to disabled from BeginPlay)
	int PlayerId = GetPlayerIdByRespawnState(ERespawnState::RSE_Hider);
	AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[PlayerId].GetController()->GetPawn());
	HISCharacter->bDisableInput = true;
	HideClone(PlayerId);
	FTimerHandle* WaitTimer = PlayersData[PlayerId].GetWaitTimer();
	GetWorldTimerManager().SetTimer(*WaitTimer, this, &AHISGameMode::IndividualSeekerRespawnWaitFinished, WaitCountdown);
	PlayersData[PlayerId].RespawnState = ERespawnState::RSE_RespawnSeekerWait;
}

void AHISGameMode::IndividualSeekerRespawnWaitFinished()
{
	int PlayerId = GetPlayerIdByRespawnState(ERespawnState::RSE_RespawnSeekerWait);
	AHISPlayerController* PlayerController = PlayersData[PlayerId].GetController();
	AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayerController->GetPawn());
	HISCharacter->bDisableInput = false;
	PlayersData[PlayerId].RespawnState = ERespawnState::RSE_NONE;
	PlayersData[PlayerId].bIsHidden = true;
}
#pragma endregion


#pragma region SPAWN
void AHISGameMode::SetSpawnLocation(int PlayerId, FVector Location, FRotator Rotation)
{
	PlayersData[PlayerId].SetSpawnPosition(Location, Rotation);
}
#pragma endregion


#pragma region HIDING
// [TODO] 
// Should we be passing AHISCharacterController??? 
// DO NOT STORE ID ON AHISCharacterHider OR AHISCharacterSeeker (but will still need to for AHISClone) ???

void AHISGameMode::SetCloneHidingLocation(class AHISPlayerController* HiderController, FVector Location, FRotator Rotation)
{
	int PlayerId = HiderController->GetPlayerId();
	PlayersData[PlayerId].SetHidePosition(Location, Rotation);

	// [TODO][IMPORTANT]
	// Keep this function, but then separate into:
	//		o SetCloneHidingLocationPreGame		-- for checking all players hidden
	//		o SetCloneHidingLocationIndividual	-- for just placing the character and clearing the (individual) timer

	if (bHasGameStarted)
	{
		SetCloneHidingLocationIndividual(PlayerId);
	}
	else
	{
		SetCloneHidingLocationPreGame();
	}
}

void AHISGameMode::SetCloneHidingLocationPreGame()
{
	// [TODO] 
	// Set camera view to either static birds-eye-view of the map, or free-cam
	// (neither of which can see Hiders or Clones. Obviously)
	// ... And delete Hider, and disable input
	// (unless for free-cam, but like for the Hider controller... Though if deleted I guess that doesn't really matter)

	bool bAllPlayersHidden = true;
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		if (PlayersData[i].bIsHidden == false)
		{
			bAllPlayersHidden = false;
			break;
		}
	}

	if (bAllPlayersHidden)
	{
		GetWorldTimerManager().ClearTimer(JointWaitTimer);

		// [TODO] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
		for (int i = 0; i < PlayersData.Num(); ++i)
		{
			// DISABLE INPUT -- Not required? Since immediately spawning a seeker (with input set to disabled from BeginPlay)
			AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
			HISCharacter->bDisableInput = true;
			HideClone(i);
			PlayersData[i].bIsHidden = true;
		}

		GetWorldTimerManager().SetTimer(JointWaitTimer, this, &AHISGameMode::JointSeekTimerFinished, WaitCountdown);
	}
}

void AHISGameMode::SetCloneHidingLocationIndividual(int PlayerId)
{
	FTimerHandle* WaitTimer = PlayersData[PlayerId].GetWaitTimer();
	GetWorldTimerManager().ClearTimer(*WaitTimer);
	PlayersData[PlayerId].RespawnState = ERespawnState::RSE_RespawnSeekerWait;

	// DISABLE INPUT -- Not required? Since immediately spawning a seeker (with input set to disabled from BeginPlay)
	AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[PlayerId].GetController()->GetPawn());
	HISCharacter->bDisableInput = true;
	HideClone(PlayerId);

	GetWorldTimerManager().SetTimer(*WaitTimer, this, &AHISGameMode::IndividualSeekerRespawnWaitFinished, WaitCountdown);
}

void AHISGameMode::HideClone(int PlayerId)
{
	PlayerGameData* PlayerData = &PlayersData[PlayerId];

	FVector PlayerLocation;
	FRotator PlayerRotation;
	if (PlayerData->bIsHidden)
	{
		PlayerLocation = PlayerData->GetHideLocation();
		PlayerRotation = PlayerData->GetHideRotation();
	}
	else
	{
		PlayerLocation = PlayerData->GetController()->GetCurrentLocation();
		PlayerRotation = PlayerData->GetController()->GetCurrentRotation();
	}

	AActor* SpawnedClone = GetWorld()->SpawnActor(CloneClass, &PlayerLocation, &PlayerRotation);
	AHISClone* Clone = Cast<AHISClone>(SpawnedClone);
	Clone->SetPlayerId(PlayerId);

	// [TODO] Also refactor the below into its own 'SpawnSeeker' class?

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
	//RestartPlayerAtPlayerStart(HiderController, PlayerStarts[Index]);

	FVector PlayerStartLocation = PlayerData->GetSpawnLocation();
	FRotator PlayerStartRotation = PlayerData->GetSpawnRotation();

	AActor* SpawnedSeeker = GetWorld()->SpawnActor(SeekerClass, &PlayerStartLocation, &PlayerStartRotation);
	AHISCharacterSeeker* Seeker = Cast<AHISCharacterSeeker>(SpawnedSeeker);

	AHISPlayerController* CurrentController = PlayerData->GetController();
	ACharacter* Character = CurrentController->GetCharacter();
	CurrentController->Possess(Seeker);
	Character->Destroy();



	// [TODO] This needs working on!!! And place it wherever we're calling SpawnActor in this script!!!
	// 
	// Think about AHISPlayerController having reference to AHISCharacter* CurrentCharacter
	// And the function inside AHISCharacter (overriden by both Hider and Seeker children) Respawn()
	// 
	// Can then, in here, call PlayersData[i].GetController()->GetCharacter()->Respawn()
	// (but don't do it with such a link - store some of the variables... Will need to cast the character for starters!)
	// Which calls RequestRespawn() in here, passing in:
	//	o Itself
	//	o Its next character type (i.e. AHISCharacterHider will want to spawn a new instance of AHISCharacterSeeker, and vice versa)
	//	o Its own controller
	// 
	// Then can continue spawning as we are doing in the RequestSpawn method
	// ... This 'convoluted way' is only for on game starting, right?
	// Will need to do like this anyway for players catching other players?
}
#pragma endregion


#pragma region SEEKING
void AHISGameMode::PlayerFound(class AHISClone* FoundClone, class AHISPlayerController* SeekerController)
{
	int FoundId = FoundClone->GetPlayerId();
	PlayersData[FoundId].RespawnState = ERespawnState::RSE_Found;
	PlayersData[FoundId].bIsHidden = false;

	// [TODO] Do not include 'FOUND' in base class?
	// Will want different things to happen to FoundCharacter depending on game mode!
	//FoundCharacter->Found();
}
#pragma endregion


#pragma region RESPAWNING
// [TODO] Save casting - change AHISPlayerController* to AController*
// [TODO] This is now incomplete! Previous signature was not passing 'NextCharacter'
//void AHISGameMode::RequestRespawn(class AHISCharacter* CurrentCharacter, class AHISCharacter* NextCharacter, class AHISPlayerController* HISPlayerController)
//void AHISGameMode::RequestRespawn(int FoundPlayerId)
void AHISGameMode::RequestRespawn()
{
	// [TODO][IMPORTANT]
	// 
	// Should have played the dissolve animation by now
	// Would then disable collision at the end?
	// 
	// That, or we could have separate 'RequestRespawnStart' and 'End' functions, where:
	//	- START
	//		o HIDER		Disables input/animation
	//					Plays freeze animation
	//					Destroys Hider character
	//					Immediately spawns Seeker
	//					If at match beginning, has player view some kind of environment camera while waiting
	//					Otherwise, or if time run out and force hide, player views Seeker
	//					STARTS 3s COUNTDOWN
	//					(enable input)
	// 
	//		o SEEKER	Disables input/animation
	//					Plays disolve animation
	//					Destroys Seeker character
	//					Immediately spawns Hider (but is frozen - greyscale the camera?) and camera is focused on that
	//					STARTS 3s COUNTDOWN
	//					(enable input)
	// 
	// ... Typing this out, I'm wondering if 'start' and 'end' are really needed? Below is incomplete if so.
	// 
	//	- END
	//		o HIDER		Enables input. 
	//					Spawns Seeker
	// 
	//		o SEEKER	Enables input.

	int PlayerId = GetPlayerIdByRespawnState(ERespawnState::RSE_Found);

	// Implement the below properly! Will need to add to the score, both for Hider and Seeker
	// (will need to create either second array or a list of int pairs)
	//PlayersData[PlayerId].AddToFoundByPlayer(5);
	
	PlayersData[PlayerId].ResetSpawn();														// [TODO] Combine and rename these two?

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
	FVector SpawnLocation = PlayerStarts[Index]->GetTransform().GetLocation();
	FRotator SpawnRotation = PlayerStarts[Index]->GetTransform().GetRotation().Rotator();

	AActor* SpawnedHider = GetWorld()->SpawnActor(HiderClass, &SpawnLocation, &SpawnRotation);
	AHISCharacterHider* Hider = Cast<AHISCharacterHider>(SpawnedHider);

	AHISPlayerController* FoundPlayerController = PlayersData[PlayerId].GetController();
	FoundPlayerController->ClientSetFoundTextVisible(false);
	ACharacter* Character = FoundPlayerController->GetCharacter();								// Not currently doing anything???
	FoundPlayerController->Possess(Hider);
	Character->Destroy();

	// [TODO] After countdown, resume control
	
	// (prev note, before setting from false to true)
	// Below temp here only for testing -- will still need to have timer to resume control, and set off new timer, etc.
	Hider->bDisableInput = true;

	FTimerHandle* PlayerTimer = PlayersData[PlayerId].GetWaitTimer();
	GetWorldTimerManager().SetTimer(*PlayerTimer, this, &AHISGameMode::IndividualHiderRespawnWaitFinished, WaitCountdown);

	PlayersData[PlayerId].RespawnState = ERespawnState::RSE_RespawnHiderWait;

	// [TODO][IMPORTANT]
	// Do not implement free-cam while waiting
	// (at least in the beginning of this development)
	// because too complex to create separate set of controls,
	// and have to switch control of different actors and spawning (etc.)
	// ...
	// Would be a nice addition, though!
	// If do eventually implement, will have to ensure we can only see Seekers
}


// [TODO][IMPORTANT]
// This function is not safe!
// Can have two or more players in e.g. RSE_Hider state
// (they have, while testing, 15s in that state. This would be even more in-game!)
// Keep track of PlayerRespawnIds (TArray) in this script,
// and have RespawnOrder number in PlayerGameData
// Then, when one Id gets removed, can reduce all existing RespawnOrder numbers by one
int AHISGameMode::GetPlayerIdByRespawnState(ERespawnState RespawnState)
{
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		if (PlayersData[i].RespawnState == RespawnState)
		{
			return i;
		}
	}
	UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::GetControllerByRespawnState] Error -- Should never be here -- return -1."));
	return -1;
}
#pragma endregion


#pragma region Update Timers
void AHISGameMode::UpdateJointWaitTimer()
{
	int SecondsRemaining = FMath::CeilToInt(GetWorldTimerManager().GetTimerRemaining(JointWaitTimer));
	if (SecondsRemaining == JointWaitTimeRemaining) return;

	JointWaitTimeRemaining = SecondsRemaining;
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		AHISPlayerController* HISController = PlayersData[i].GetController();
		if (HISController)
		{
			HISController->ClientUpdateHUDTimer(JointWaitTimeRemaining);
		}
	}
}

void AHISGameMode::UpdatePlayerHUDCountdown(int PlayerId)
{
	FTimerHandle* PlayerTimer = PlayersData[PlayerId].GetWaitTimer();
	int SecondsRemaining = FMath::CeilToInt(GetWorldTimerManager().GetTimerRemaining(*PlayerTimer));

	AHISPlayerController* HISController = PlayersData[PlayerId].GetController();
	// [Q] Need to null check the controller?
	if (HISController)
	{
		HISController->ClientUpdateHUDTimer(SecondsRemaining);
	}
}

void AHISGameMode::UpdatePlayerHUDGameTimer(int PlayerId)
{
	AHISPlayerController* HISController = PlayersData[PlayerId].GetController();
	// [Q] Need to null check the controller?
	if (HISController)
	{
		HISController->ClientUpdateHUDTimer(CurrentGameTime);
	}
}

void AHISGameMode::CheckGameTimer()
{
	int ElapsedSeconds = FMath::FloorToInt(GetWorldTimerManager().GetTimerElapsed(GameTimer));

	// [TODO]
	//if (ElapsedSeconds > GameTimeLimit)
	//{
	//	EndGame();
	//}

	if (ElapsedSeconds == CurrentGameTime) return;
	CurrentGameTime = ElapsedSeconds;
}
#pragma endregion
