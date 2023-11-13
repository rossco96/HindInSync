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

	if (bIsShowingCountdown)
	{
		UpdatePlayerCountdown();
		return;
	}

	if (bHasGameStarted)
	{
		UpdatePlayersGameTimers();
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
		StartWaitTimer(WaitCountdown, &AHISGameMode::PreGameCountdownFinished);
	}
}
#pragma endregion


#pragma region TIMERS
void AHISGameMode::StartWaitTimer(float Duration, FSimpleDelegate::TMethodPtr<AHISGameMode> OnComplete)
{
	bIsShowingCountdown = true;
	GetWorldTimerManager().SetTimer(WaitTimer, this, OnComplete, Duration);
}

// [NOTE] Below not currently used
void AHISGameMode::StartWaitTimerWithDisableInput(float Duration, FSimpleDelegate::TMethodPtr<AHISGameMode> OnComplete, AHISPlayerController* HISController, bool DisableInput)
{
	GetWorldTimerManager().SetTimer(WaitTimer, this, OnComplete, Duration);
	AHISCharacter* HISCharacter = Cast<AHISCharacter>(HISController->GetPawn());
	HISCharacter->bDisableInput = DisableInput;
}

void AHISGameMode::StartGameTimer()
{
	//float MaxTime = std::numeric_limits<float>::max();					// Why does this return a negative number?
	float MaxTime = 9999999999.9f;											// [TODO] HACK (and ~317 years, for anyone interested) at the very least turn this into a const MAX_TIME
	//UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::StartGameTimer] GAME TIMER -- %d"), MaxTime);
	GetWorldTimerManager().SetTimer(GameTimer, MaxTime, false);
	bHasGameStarted = true;
}
#pragma endregion


#pragma region (OnTimersComplete)
void AHISGameMode::PreGameCountdownFinished()
{
	bIsShowingCountdown = false;
	
	// [TODO][Q] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers? What if people drop out!?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
		HISCharacter->bDisableInput = false;
	}

	StartWaitTimer(HideTimeLimit, &AHISGameMode::InitialHideTimerFinished);
}

void AHISGameMode::InitialHideTimerFinished()
{
	// If NOT all players hidden by now, must force hide remaining players!
	
	// [TODO] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		// DISABLE INPUT -- Not required? Since immediately spawning a seeker (with input set to disabled from BeginPlay)
		AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
		HISCharacter->bDisableInput = true;
		HideClone(i);
	}

	StartWaitTimer(WaitCountdown, &AHISGameMode::InitialSeekTimerFinished);
}

void AHISGameMode::InitialSeekTimerFinished()
{
	bIsShowingCountdown = false;

	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		// ENABLE INPUT
		AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
		HISCharacter->bDisableInput = false;
	}

	StartGameTimer();
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
	PlayerGameData* HiderPlayerData = &PlayersData[PlayerId];					// Interesting.. Using a pointer here... Should I be doing like this elsewhere? Do I NEED to do it like this here/elsewhere?
	HiderPlayerData->SetHidePosition(Location, Rotation);

	// [TODO] 
	// Set camera view to either static birds-eye-view of the map, or free-cam
	// (neither of which can see Hiders or Clones. Obviously)
	// ... And delete Hider, and disable input
	// (unless for free-cam, but like for the Hider controller... Though if deleted I guess that doesn't really matter)

	bool bAllPlayersHidden = true;
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		if (PlayersData[i].IsPlayerHidden() == false)
		{
			bAllPlayersHidden = false;
			break;
		}
	}

	if (bAllPlayersHidden)
	{
		GetWorldTimerManager().ClearTimer(WaitTimer);
		
		// [TODO] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
		for (int i = 0; i < PlayersData.Num(); ++i)
		{
			// DISABLE INPUT -- Not required? Since immediately spawning a seeker (with input set to disabled from BeginPlay)
			AHISCharacter* HISCharacter = Cast<AHISCharacter>(PlayersData[i].GetController()->GetPawn());
			HISCharacter->bDisableInput = true;
			HideClone(i);
		}

		StartWaitTimer(WaitCountdown, &AHISGameMode::InitialSeekTimerFinished);
	}
}


void AHISGameMode::HideClone(int PlayerId)
{
	FVector PlayerLocation;
	FRotator PlayerRotation;
	if (PlayersData[PlayerId].IsPlayerHidden())
	{
		PlayerLocation = PlayersData[PlayerId].GetHideLocation();
		PlayerRotation = PlayersData[PlayerId].GetHideRotation();
	}
	else
	{
		// [IMPORTANT]
		// PLAYERS DATA DOES NOT CURRENTLY HAVE REFERENCE TO HISCharacterHider (or Seeker, or Clone)
		// There is in fact no data in HISGameMode which currently points to any of that
		// ... Should include in PlayerGameData? Or include separately?
		PlayerLocation = PlayersData[PlayerId].GetController()->GetCurrentLocation();
		PlayerRotation = PlayersData[PlayerId].GetController()->GetCurrentRotation();
	}

	AActor* SpawnedClone = GetWorld()->SpawnActor(CloneClass, &PlayerLocation, &PlayerRotation);
	AHISClone* Clone = Cast<AHISClone>(SpawnedClone);
	Clone->SetPlayerId(PlayerId);

	// [TODO] Also refactor the below into its own 'SpawnSeeker' class?

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
	//RestartPlayerAtPlayerStart(HiderController, PlayerStarts[Index]);

	FVector PlayerStartLocation = PlayersData[PlayerId].GetSpawnLocation();
	FRotator PlayerStartRotation = PlayersData[PlayerId].GetSpawnRotation();

	AActor* SpawnedSeeker = GetWorld()->SpawnActor(SeekerClass, &PlayerStartLocation, &PlayerStartRotation);
	AHISCharacterSeeker* Seeker = Cast<AHISCharacterSeeker>(SpawnedSeeker);

	AHISPlayerController* CurrentController = PlayersData[PlayerId].GetController();
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
	int CharacterIndex = FoundClone->GetPlayerId();
	//PlayersData[CharacterIndex].

	// [TODO] Do not include 'FOUND' in base class?
	// Will want different things to happen to FoundCharacter depending on game mode!
	//FoundCharacter->Found();
	
	// [TODO][Important]
	// Need to find a way of "Finding" the linked active player as well...
	//		- Do via FoundCharacter->Found() ?
	//		OR 
	//		- Do via HISGameMode (e.g. lookup the ActivePlayer corresponding to this FoundCharacter, somehow, and send message like that)
}
#pragma endregion


#pragma region RESPAWNING
// [TODO] Save casting - change AHISPlayerController* to AController*
// [TODO] This is now incomplete! Previous signature was not passing 'NextCharacter'
//void AHISGameMode::RequestRespawn(class AHISCharacter* CurrentCharacter, class AHISCharacter* NextCharacter, class AHISPlayerController* HISPlayerController)
void AHISGameMode::RequestRespawn(int FoundPlayerId)
{
	//PlayersData[FoundPlayerId].SetPlayerFound();



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


	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
	FVector SpawnLocation = PlayerStarts[Index]->GetTransform().GetLocation();
	FRotator SpawnRotation = PlayerStarts[Index]->GetTransform().GetRotation().Rotator();

	AActor* SpawnedHider = GetWorld()->SpawnActor(HiderClass, &SpawnLocation, &SpawnRotation);
	AHISCharacterHider* Hider = Cast<AHISCharacterHider>(SpawnedHider);

	AHISPlayerController* FoundPlayerController = PlayersData[FoundPlayerId].GetController();
	ACharacter* Character = FoundPlayerController->GetCharacter();
	FoundPlayerController->Possess(Hider);

	// [TODO] After countdown, resume control
	
	UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::InitialCountdownFinished] ~WaitTimer~ (since-deleted call to resumeplayercontrol"));
	//FoundPlayerController->ResumePlayerControl();							// COMMENTED OUT FOR TESTING ONLY?

	
	/*
	if (CurrentCharacter)
	{
		CurrentCharacter->Reset();
		CurrentCharacter->Destroy();		// [TODO] Is this also needed? Doesn't Reset() deal with destroying something?
	}

	if (HISPlayerController)
	{
		// [TODO][IMPORTANT]
		// Remember, we're wanting the Hider to spawn in a unique place each time
		// BUT, we're wanting the Seeker to spawn in the same place as the Hider!
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(HISPlayerController, PlayerStarts[Index]);
	}
	//*/


	// [TODO][IMPORTANT]
	// Do not implement free-cam while waiting
	// (at least in the beginning of this development)
	// because too complex to create separate set of controls,
	// and have to switch control of different actors and spawning (etc.)
	// ...
	// Would be a nice addition, though!
}
#pragma endregion


void AHISGameMode::UpdatePlayerCountdown()
{
	int SecondsRemaining = FMath::FloorToInt(GetWorldTimerManager().GetTimerRemaining(WaitTimer));
	if (SecondsRemaining == CurrentWaitTime) return;

	CurrentWaitTime = SecondsRemaining;
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		AHISPlayerController* HISController = PlayersData[i].GetController();
		if (HISController)
		{
			HISController->ClientUpdateHUDTimer(SecondsRemaining);
		}
		else
		{
			UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::UpdatePlayerCountdown] HISController == nullptr <<< <<< <<< <<< <<<"));
		}
	}
}

void AHISGameMode::UpdatePlayersGameTimers()
{
	int ElapsedSeconds = FMath::FloorToInt(GetWorldTimerManager().GetTimerElapsed(GameTimer));
	if (ElapsedSeconds == CurrentGameTime) return;

	CurrentGameTime = ElapsedSeconds;
	// [TODO][Q] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		AHISPlayerController* HISController = PlayersData[i].GetController();
		// [Q] Need to null check the controller?
		HISController->ClientUpdateHUDTimer(ElapsedSeconds);
	}
}
