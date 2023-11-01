// Fill out your copyright notice in the Description page of Project Settings.


#include "HISGameMode.h"
#include "GameFramework/GameStateBase.h"							// [TODO] GameStateBase needed for GameState.Get() ONLY
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

	/*
	GetWorldTimerManager().SetTimer(
		HideTimer,
		this,
		&AHISGameMode::HideTimerFinished,
		HideTimeLimit
	);
	//*/

	//UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::BeginPlay] ~Timer~"));
	//StartWaitTimer(HideTimeLimit, AHISGameMode::HideTimerFinished);
}


void AHISGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHasGameStarted)
	{
		UpdatePlayerTimers();
	}
}


void AHISGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::PostLogin] <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< (call to FreezePlayerControl)"));

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	
	// Id should be zero-based index
	int32 PlayerId = NumberOfPlayers - 1;
	
	AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(NewPlayer);
	//HISPlayerController->FreezePlayerControl();											// THIS IS NOT WORKING? WHY???
	HISPlayerController->SetPlayerId(PlayerId);
	PlayerGameData Data = PlayerGameData(HISPlayerController);

	AHISCharacterHider* HISHider = Cast<AHISCharacterHider>(HISPlayerController->GetCharacter());
	if (HISHider)
	{
		Data.SetSpawnPosition(HISHider->GetLocation(), HISHider->GetRotation());
	}
	else
	{
		// DEBUG ONLY
		UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::PostLogin] HISHider == nullptr (presumably for server-client only?)"));
	}

	//HISCharacterClone Player = (HISCharacterClone)PlayerArray[PlayerId];					// [TODO] Should use pointer here, no?

	// [TODO] want to assign this new player to PlayersData
	// zero-based indexing please!
	//PlayersData.Add(PlayerId, PlayerGameData(PlayerId));									// OLD WAY OF DOING THIS
	PlayersData.Add(PlayerId, Data);

	
	// [TODO] want to start countdown once target reached
	/*
	if (PlayerId + 1 == TargetNumberOfPlayers)
	{
		// [TODO] Set HISCharacterClone PlayerId
		// (and assigned PlayerStart..? Can we do that here???)
		//
		// [TODO] Call to initialise all player arrays
		//
		// [TODO] Move to HIDE gamestate, with countdown
	}
	//*/


	if (NumberOfPlayers == TEST_NumberOfPlayers)
	{
		// [TEST] Below here, rather than above
		for (int i = 0; i < NumberOfPlayers; ++i)
		{
			//PlayersData[i].GetController()->FreezePlayerControl();
			PlayersData[i].GetController()->SetPlayerControl(true);
		}

		// START THE COUNTDOWN!
		// ... Freeze controllers in here?
		UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::PostLogin] ~WaitTimer~"));
		StartWaitTimer(WaitCountdown, &AHISGameMode::InitialCountdownFinished);
	}
}
#pragma endregion


#pragma region TIMERS
void AHISGameMode::StartWaitTimer(float Duration, FSimpleDelegate::TMethodPtr<AHISGameMode> OnComplete)
{
	UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::StartWaitTimer] ~WaitTimer~"));
	GetWorldTimerManager().SetTimer(WaitTimer, this, OnComplete, Duration);
}

// [NOTE] Below not currently used
void AHISGameMode::StartWaitTimerWithInputEnabled(float Duration, FSimpleDelegate::TMethodPtr<AHISGameMode> OnComplete, AHISPlayerController* HISController, bool InputEnabled)
{
	GetWorldTimerManager().SetTimer(WaitTimer, this, OnComplete, Duration);
	//if (InputEnabled)	HISController->ResumePlayerControl();
	//else				HISController->FreezePlayerControl();
	if (InputEnabled)	HISController->SetPlayerControl(false);
	else				HISController->SetPlayerControl(true);
}

void AHISGameMode::StartGameTimer()
{
	//float MaxTime = std::numeric_limits<float>::max();
	//float MaxTime = std::numeric_limits<float>::lowest();
	float MaxTime = 9999999999.9f;											// [TODO] HACK (and ~317 years, for anyone interested)
	//UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::StartGameTimer] GAME TIMER -- %d"), MaxTime);
	GetWorldTimerManager().SetTimer(GameTimer, MaxTime, false);
	bHasGameStarted = true;
}
#pragma endregion


#pragma region (OnTimersComplete)
void AHISGameMode::InitialCountdownFinished()
{
	UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::InitialCountdownFinished] ~WaitTimer~ CALL TO RESUMEPLAYERCONTROL"));

	// [TODO][Q] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		//PlayersData[i].GetController()->ResumePlayerControl();
		PlayersData[i].GetController()->SetPlayerControl(false);;
	}

	StartWaitTimer(HideTimeLimit, &AHISGameMode::InitialHideTimerFinished);
}

void AHISGameMode::InitialHideTimerFinished()
{
	UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::InitialHideTimerFinished] ~WaitTimer~"));

	// If NOT all players hidden by now, must force hide remaining players!
	
	// [TODO] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		HideClone(i);
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
		//GetWorldTimerManager().ClearTimer(HideTimer);
		GetWorldTimerManager().ClearTimer(WaitTimer);
		
		for (int i = 0; i < PlayersData.Num(); ++i)
		{
			HideClone(i);
		}

		// [TODO]
		// This is still temp! But closer.
		// Will want to change game state, and do inside there instead!
		// [NOTE]
		// The 'TODO' comment above related to code previously below, now refactored into HideAllClones()

		// [TODO]
		// Will need to conisder what doing about re-hiding. This is very much aimed at pre-game and will need expanding!
	}
}


void AHISGameMode::HideClone(int PlayerId)
{
	FVector PlayerLocation;
	FRotator PlayerRotation;
	if (PlayersData[PlayerId].IsPlayerHidden() == false)
	{
		// AHHH
		// PLAYERS DATA DOES NOT CURRENTLY HAVE REFERENCE TO HISCharacterHider (or Seeker, or Clone)
		// There is in fact no data in HISGameMode which currently points to any of that
		// ... Should include in PlayerGameData? Or include separately?
		PlayerLocation = PlayersData[PlayerId].GetController()->GetCurrentLocation();
		PlayerRotation = PlayersData[PlayerId].GetController()->GetCurrentRotation();
	}
	else
	{
		PlayerLocation = PlayersData[PlayerId].GetHideLocation();
		PlayerRotation = PlayersData[PlayerId].GetHideRotation();
	}

	AActor* SpawnedClone = GetWorld()->SpawnActor(CloneClass, &PlayerLocation, &PlayerRotation);
	AHISClone* Clone = Cast<AHISClone>(SpawnedClone);
	Clone->SetPlayerId(PlayerId);



	// [TODO][IMPORTANT]
	// Make sure Seeker N is spawning where Hider N spawned,
	// not just at some random PlayerStart (though this will do for now!)
	// OR!
	// Eventually allow as an option?
	// ('spawn in same spot? y/n')

	// [TODO] Also refactor the below into its own 'SpawnSeeker' class?

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
	//RestartPlayerAtPlayerStart(HiderController, PlayerStarts[Index]);

	//FVector PlayerStartLocation = PlayerStarts[Index]->GetTransform().GetLocation();					// [TODO] Potensh enable
	//FRotator PlayerStartRotation = PlayerStarts[Index]->GetTransform().GetRotation().Rotator();		// this as an option
	FVector PlayerStartLocation = PlayersData[PlayerId].GetSpawnLocation();
	FRotator PlayerStartRotation = PlayersData[PlayerId].GetSpawnRotation();

	AActor* SpawnedSeeker = GetWorld()->SpawnActor(SeekerClass, &PlayerStartLocation, &PlayerStartRotation);
	AHISCharacterSeeker* Seeker = Cast<AHISCharacterSeeker>(SpawnedSeeker);
	Seeker->SetPlayerId(PlayerId);

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

	//RequestRespawn()
}
#pragma endregion


#pragma region SEEKING
void AHISGameMode::PlayerFound(class AHISClone* FoundClone, /*class AHISPlayerController* HiderController,*/ class AHISPlayerController* SeekerController)
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
	Hider->SetPlayerId(FoundPlayerId);

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


void AHISGameMode::UpdatePlayerTimers()
{
	// [TODO][Q] Instead of using PlayersData.Num() should be using (yet to be included) TotalPlayers?
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		float ElapsedSeconds = GetWorldTimerManager().GetTimerElapsed(GameTimer);
		
		// vvv DEBUG vvv
		//FString DebugExists = (GetWorldTimerManager().TimerExists(GameTimer)) ? "Y" : "N";
		//FString DebugActive = (GetWorldTimerManager().IsTimerActive(GameTimer)) ? "Y" : "N";
		//UE_LOG(LogActor, Warning, TEXT("[UCharacterOverlay::UpdateTimer] Exists / Active / ElapsedSeconds --> %s / %s / %d"), *DebugExists, *DebugActive, ElapsedSeconds);
		// ^^^ DEBUG ^^^
		
		AHISPlayerController* HISController = PlayersData[i].GetController();
		// [Q] Need to null check the controller?
		HISController->ClientRPCUpdateHUDTimer(ElapsedSeconds);
	}
}
