// Fill out your copyright notice in the Description page of Project Settings.


#include "HISGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "HideInSync/Character/HISCharacterHider.h"
#include "HideInSync/Character/HISClone.h"
#include "HideInSync/GameInstance/HISGameInstance.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "HideInSync/PlayerState/HISPlayerState.h"


#pragma region UNREAL FUNCTIONS
AHISGameMode::AHISGameMode()
{
	bDelayedStart = true;
}

void AHISGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();

	// [TODO] Store HISGameInstance rather than casting each time!
	HISGameInstance = Cast<UHISGameInstance>(GetGameInstance());
	GameTimeLimit = HISGameInstance->GetGameTimeLimit() * 60;			// Multiply by 60 as measured in minutes [TODO] Include the multiply in GetGameTimeLimit() ???
	HideTimeLimit = HISGameInstance->GetHideTimeLimit();
	RespawnWaitTime = HISGameInstance->GetRespawnTime();
	
	bIsGameTimeEndless = (GameTimeLimit == 0);
	bIsHideTimeEndless = (HideTimeLimit == 0);
	bNoRespawnTime = (RespawnWaitTime == 0);
}

void AHISGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MatchState == MatchState::WaitingToStart)
	{
		CurrentGameStartCountdown = LevelStartingTime + GameStartWaitTime - GetWorld()->GetTimeSeconds();
		if (CurrentGameStartCountdown <= 0.0f)
		{
			StartMatch();
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
		for (int i = 0; i < NumberOfPlayers; ++i)
		{
			// INFORM ALL PLAYER CONTROLLERS THAT PLAYERS ARE LOADED IN
			PlayersData[i].GetController()->TotalNumberOfPlayers = NumberOfPlayers;
			PlayersData[i].GetController()->bAllPlayersLoadedIn = true;
			// INIT SCORES MAP:
			TMap<int, int> Score = TMap<int, int>();
			for (int j = 0; j < NumberOfPlayers; ++j)
			{
				if (i == j) continue;
				Score.Add(j, 0);
			}
			GameScore.Add(i, Score);
		}
	}
}

void AHISGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(*Iterator);
		if (HISPlayerController)
		{
			HISPlayerController->OnMatchStateSet(MatchState);
		}
	}
}
#pragma endregion


#pragma region SPAWN
void AHISGameMode::SetSpawnLocation(int PlayerId, FVector Location, FRotator Rotation)
{
	PlayersData[PlayerId].SetSpawnPosition(Location, Rotation);
}


/*
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

	int PlayerId = GetPlayerIdByRespawnState(ERespawnState::Found);

	// Implement the below properly! Will need to add to the score, both for Hider and Seeker
	// (will need to create either second array or a list of int pairs)

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

	//FoundPlayerController->ClientUpdateHUDTimerLabel(ETimerLabelState::WAIT);					// [IMPORTANT] Moved to child game mode!

	// [TODO] After countdown, resume control

	// (prev note, before setting from false to true)
	// Below temp here only for testing -- will still need to have timer to resume control, and set off new timer, etc.
	Hider->bDisableInput = true;

	FTimerHandle* PlayerTimer = PlayersData[PlayerId].GetWaitTimer();
	PlayersData[PlayerId].RespawnState = ERespawnState::RespawnHiderWait;

	if (bNoRespawnTime)
		IndividualHiderRespawnWaitFinished();
	else
		GetWorldTimerManager().SetTimer(*PlayerTimer, this, &AHISGameMode::IndividualHiderRespawnWaitFinished, RespawnWaitTime);	// Previously WaitCountdown

	// [TODO][IMPORTANT]
	// Do not implement free-cam while waiting
	// (at least in the beginning of this development)
	// because too complex to create separate set of controls,
	// and have to switch control of different actors and spawning (etc.)
	// ...
	// Would be a nice addition, though!
	// If do eventually implement, will have to ensure we can only see Seekers
}
//*/


int AHISGameMode::GetPlayerIdByRespawnState(ERespawnState RespawnState)
{
	int Index = -1;
	int LowestRespawnOrderId = INT32_MAX;
	for (int i = 0; i < PlayersData.Num(); ++i)
	{
		if (PlayersData[i].RespawnOrderId < 0) continue;
		if (PlayersData[i].RespawnState == RespawnState && PlayersData[i].RespawnOrderId < LowestRespawnOrderId)
		{
			Index = i;
			LowestRespawnOrderId = PlayersData[i].RespawnOrderId;
		}
	}
	if (Index == -1)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISGameMode::GetControllerByRespawnState] Error -- Should never be here -- returning -1."));
	}
	return Index;
}
#pragma endregion


#pragma region HIDING
void AHISGameMode::CheckAllClonesHidden()
{
	bool AllClonesHidden = true;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(*Iterator);
		if (HISPlayerController->bHidePositionSet == false)
		{
			AllClonesHidden = false;
			break;
		}
	}

	if (AllClonesHidden)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AHISPlayerController* HISPlayerController = Cast<AHISPlayerController>(*Iterator);
			SpawnHiddenClone(HISPlayerController);
		}
	}
}

void AHISGameMode::SpawnHiddenClone(AHISPlayerController* HISPlayerController)
{
	if (bNoRespawnTime)
		HISPlayerController->OnRespawnStateSet(ERespawnState::NONE);
	else
		HISPlayerController->OnRespawnStateSet(ERespawnState::RespawnSeekerWait);
}

/*
void AHISGameMode::HideClone(int PlayerId)
{
	PlayerGameData* PlayerData = &PlayersData[PlayerId];

	FVector PlayerLocation;
	FRotator PlayerRotation;
	if (PlayerData->bHidingPlaceSet)
	{
		PlayerLocation = PlayerData->GetHideLocation();
		PlayerRotation = PlayerData->GetHideRotation();
	}
	else
	{
		PlayerLocation = PlayerData->GetController()->GetCurrentLocation();
		PlayerRotation = PlayerData->GetController()->GetCurrentRotation();
		PlayerData->bHidingPlaceSet = true;
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
//*/
#pragma endregion


#pragma region SEEKING
void AHISGameMode::PlayerFound(class AHISClone* FoundClone, class AHISPlayerController* SeekerController)
{
	//int FoundId = FoundClone->GetPlayerId();
	int FoundId = FoundClone->HISPlayerController->GetPlayerId();
	
	// [TODO] Move this into a "ResetHidingData()" function within PlayerGameData ???
	PlayersData[FoundId].RespawnState = ERespawnState::Found;
	PlayersData[FoundId].bHidingPlaceSet = false;
	PlayersData[FoundId].bIsHidden = false;
	PlayersData[FoundId].RespawnOrderId = CurrentRespawnOrderId;

	CurrentRespawnOrderId++;

	AHISPlayerState* SeekerPlayerState = (SeekerController) ? Cast<AHISPlayerState>(SeekerController->PlayerState) : nullptr;
	// [TODO][IMPORTANT] Must pass HiderController in this method!!!
	//AHISPlayerState* HiderPlayerState = (HiderController) ? Cast<AHISPlayerState>(HiderController->PlayerState) : nullptr;

	if (SeekerPlayerState)
	{
		SeekerPlayerState->AddToScore(1.0f);
	}

	// [TODO] Do not include 'FOUND' in base class?
	// Will want different things to happen to FoundCharacter depending on game mode!
	//FoundCharacter->Found();

	int SeekerId = SeekerController->GetPlayerId();
	UE_LOG(LogActor, Warning, TEXT("[HISGameMode::PlayerFound] SeekerId %d / FoundId %d"), SeekerId, FoundId);

	GameScore[SeekerId][FoundId]++;

	// Update Seeker HUD
	int SeekerScoreSlot = NumPlayers;								// Starting point, ignoring row and column titles, minus one since there is no top left slot!
	SeekerScoreSlot += (SeekerId > FoundId) ? FoundId : FoundId - 1;
	UE_LOG(LogActor, Warning, TEXT("[HISGameMode::PlayerFound] SeekerScoreSlot %d"), SeekerScoreSlot);
	SeekerController->ClientUpdateHUDScores(SeekerScoreSlot, GameScore[SeekerId][FoundId]);

	// Update Hider HUD
	int HiderScoreSlot = 2 * NumPlayers;
	HiderScoreSlot += (SeekerId > FoundId) ? SeekerId - 1 : SeekerId;
	UE_LOG(LogActor, Warning, TEXT("[HISGameMode::PlayerFound] HiderScoreSlot %d"), HiderScoreSlot);
	PlayersData[FoundId].GetController()->ClientUpdateHUDScores(HiderScoreSlot, GameScore[SeekerId][FoundId]);	// [TODO] This is hacky to access the controller like this... Pass it to the function!
}
#pragma endregion
