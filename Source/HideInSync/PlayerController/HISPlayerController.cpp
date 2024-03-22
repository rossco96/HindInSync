// Fill out your copyright notice in the Description page of Project Settings.


#include "HISPlayerController.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "HideInSync/Character/HISCharacter.h"
#include "HideInSync/Character/HISClone.h"
#include "HideInSync/GameInstance/HISGameInstance.h"
#include "HideInSync/GameMode/HISGameMode.h"
#include "HideInSync/HUD/Levels/CharacterOverlay.h"
#include "HideInSync/HUD/Levels/HISHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
void AHISPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HISHUD = Cast<AHISHUD>(GetHUD());

	ServerCheckMatchState();
}

void AHISPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHISPlayerController, PlayerId);
	DOREPLIFETIME(AHISPlayerController, bAllPlayersLoadedIn);
	DOREPLIFETIME(AHISPlayerController, TotalNumberOfPlayers);
	DOREPLIFETIME(AHISPlayerController, MatchState);
	DOREPLIFETIME(AHISPlayerController, RespawnState);
}

void AHISPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckTimeSync(DeltaTime);
	SetHUDTime();
	if (MatchState == MatchState::InProgress && RespawnState != ERespawnState::NONE)
	{
		CheckSoloTimer();
	}
	
	PollInit();
}

void AHISPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (HISHUD && HISHUD->CharacterOverlay)
		{
			CharacterOverlay = HISHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				//SetHUDScore(HUDScore);
				SetHUDGameTimerText(HUDCountdownTime);			//HUDAnnouncementTime		// Can delete this or nah?
			}
		}
	}

	// [NOTE] Should never need to be in here!
	// HISGameInstance should always be set in ServerCheckMatchState()
	// ... Also, this would not replicate to the clients? Would need these as replicated vars?
	if (HasAuthority() && HISGameInstance == nullptr)
	{
		HISGameInstance = Cast<UHISGameInstance>(GetGameInstance());
		if (HISGameInstance)
		{
			GameTimeLimit = HISGameInstance->GetGameTimeLimit() * 60;			// Multiply by 60 as measured in minutes [TODO] Include the multiply in GetGameTimeLimit() ???
			HideTimeLimit = HISGameInstance->GetHideTimeLimit();
			RespawnWaitTime = HISGameInstance->GetRespawnTime();

			bIsGameTimeEndless = (GameTimeLimit == 0);
			bIsHideTimeEndless = (HideTimeLimit == 0);
			bNoRespawnTime = (RespawnWaitTime == 0);
		}
	}

	// [NOTE][HACK?][14.12.2023]
	if (bHUDScoresInitialised == false)
	{
		// Is the check for TotalNumberOfPlayers required? Use OnRep_TotalNumberOfPlayers() instead! Then no need for checking in Tick(), nor any need for bHUDScoresInitialised
		if (IsLocalPlayerController() && bAllPlayersLoadedIn && TotalNumberOfPlayers != -1)
		{
			HISHUD = Cast<AHISHUD>(GetHUD());
			if (HISHUD && HISHUD->CharacterOverlay)
			{
				HISHUD->InitScorePanel(TotalNumberOfPlayers);
				bHUDScoresInitialised = true;
			}
		}
	}
}
#pragma endregion // UNREAL FUNCTIONS


#pragma region HUD
void AHISPlayerController::SetHUDTime()
{
	float TimeLeft = 0.0f;
	uint32 SecondsLeft = 0;

	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = PreGameWaitTime - GetServerTime() + LevelStartingTime;
		SecondsLeft = FMath::CeilToInt(TimeLeft);
	}
	else if (MatchState == MatchState::InProgress)
	{
		switch (RespawnState)
		{
		case ERespawnState::RespawnHiderWait:
		case ERespawnState::RespawnSeekerWait:
			TimeLeft = SoloTimerLimit - GetServerTime() + SoloTimerStartTime;
			SecondsLeft = FMath::CeilToInt(TimeLeft);							// CeilToInt for countDOWN
			break;

		case ERespawnState::Hider:
			TimeLeft = (bIsHideTimeEndless)
				? GetServerTime() - SoloTimerStartTime
				: SoloTimerLimit - GetServerTime() + SoloTimerStartTime;
			SecondsLeft = (bIsHideTimeEndless)
				? FMath::FloorToInt(TimeLeft)				// FloorToInt for countUP
				: FMath::CeilToInt(TimeLeft);				// CeilToInt for countDOWN
			break;

		case ERespawnState::NONE:
			TimeLeft = (bIsGameTimeEndless)
				? GetServerTime() - GameTimerStartingTime
				: GameTimeLimit - GetServerTime() + GameTimerStartingTime;
			SecondsLeft = (bIsGameTimeEndless)
				? FMath::FloorToInt(TimeLeft)				// FloorToInt for countUP
				: FMath::CeilToInt(TimeLeft);				// CeilToInt for countDOWN
			break;

		case ERespawnState::Found:
		case ERespawnState::Spectate:
		default:
			// If Found, keep TimeLeft at 0.0f
			// [TODO] If Spectate, game mode for this is not currently handled
			break;
		}
	}

	if (GameTimeInt == SecondsLeft) return;

	if (MatchState == MatchState::WaitingToStart)
		SetHUDAnnoucementTimerText(TimeLeft);
	else if (MatchState == MatchState::InProgress)
		SetHUDGameTimerText(TimeLeft);

	GameTimeInt = SecondsLeft;
}


/*
void AHISPlayerController::SetHUDScore(float Score)
{
	HISHUD = (HISHUD == nullptr) ? Cast<AHISHUD>(GetHUD()) : HISHUD;
	if (HISHUD && HISHUD->CharacterOverlay && HISHUD->CharacterOverlay->ScoreAmountText)
	{
		FString ScoreAmount = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		HISHUD->CharacterOverlay->ScoreAmountText->SetText(FText::FromString(ScoreAmount));
	}
	else
	{
		bInitialiseCharacterOverlay = true;
		HUDScore = Score;
	}
}
//*/


void AHISPlayerController::SetHUDAnnoucementTimerText(float CountdownTime)
{
	HISHUD = (HISHUD == nullptr) ? Cast<AHISHUD>(GetHUD()) : HISHUD;
	if (HISHUD && HISHUD->AnnouncementWidget && HISHUD->AnnouncementWidget->CountdownTimer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - (Minutes * 60);
		FString TimerText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HISHUD->AnnouncementWidget->CountdownTimer->SetText(FText::FromString(TimerText));
	}
	else
	{
		bInitialiseCharacterOverlay = true;
		HUDAnnouncementTime = CountdownTime;
	}
}

void AHISPlayerController::SetHUDTimerLabel(ETimerLabelState TimerState)
{
	if (HISHUD)
	{
		if (HISHUD->CharacterOverlay)
		{
			// [TODO] HACK!!!!! This is a proof of concept ONLY!!!
			FName TimerStateName = UEnum::GetValueAsName<ETimerLabelState>(TimerState);
			FString LeftString = "";
			FString RightString = "";
			TimerStateName.ToString().Split("::", &LeftString, &RightString);
			FString TimerLabel = RightString + ":";

			HISHUD->CharacterOverlay->UpdateTimerLabel(TimerLabel);
		}
	}
}

/*
void AHISPlayerController::SetFoundTextVisible(bool bIsVisible)
{
	if (HISHUD && HISHUD->CharacterOverlay)
	{
		HISHUD->CharacterOverlay->SetFoundTextVisible(bIsVisible);
	}
}
//*/


void AHISPlayerController::SetHUDGameTimerText(float CountdownTime)
{
	HISHUD = (HISHUD == nullptr) ? Cast<AHISHUD>(GetHUD()) : HISHUD;
	if (HISHUD && HISHUD->CharacterOverlay && HISHUD->CharacterOverlay->GameTimer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - (Minutes * 60);
		FString TimerText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HISHUD->CharacterOverlay->GameTimer->SetText(FText::FromString(TimerText));
	}
	else
	{
		bInitialiseCharacterOverlay = true;
		HUDCountdownTime = CountdownTime;
	}
}
#pragma endregion // HUD


#pragma region Timer Sync
void AHISPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AHISPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime >= TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}

// Called on the client, executed on the server
void AHISPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

// Called on the server, executed on the client
void AHISPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceiveClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceiveClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AHISPlayerController::GetServerTime()
{
	if (HasAuthority())
		return GetWorld()->GetTimeSeconds();
	else
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}


void AHISPlayerController::CheckSoloTimer()
{
	float TimeLeft = 0.0f;

	switch (RespawnState)
	{
	case ERespawnState::RespawnHiderWait:
	case ERespawnState::RespawnSeekerWait:
		TimeLeft = SoloTimerLimit - GetServerTime() + SoloTimerStartTime;
		if (TimeLeft < 0.0f)
		{
			OnRespawnStateSet
			(
				(RespawnState == ERespawnState::RespawnHiderWait) ? ERespawnState::Hider : ERespawnState::NONE
			);
		}
		break;

	case ERespawnState::Hider:
		TimeLeft = (bIsHideTimeEndless)
			? GetServerTime() - SoloTimerStartTime
			: SoloTimerLimit - GetServerTime() + SoloTimerStartTime;
		if (bIsHideTimeEndless == false && TimeLeft < 0.0f)
		{
			OnRespawnStateSet
			(
				(bNoRespawnTime) ? ERespawnState::NONE : ERespawnState::RespawnSeekerWait
			);
		}
		break;

	case ERespawnState::Found:
		// [TODO][IMPORTANT] Temp here!
		// Will eventually have nothing here and will be controlled by on found anim finished
		TimeLeft = SoloTimerLimit - GetServerTime() + SoloTimerStartTime;
		if (TimeLeft <= 0.0f)
		{
			OnRespawnStateSet
			(
				(bNoRespawnTime) ? ERespawnState::Hider : ERespawnState::RespawnHiderWait
			);
		}
		break;
	case ERespawnState::Spectate:
	case ERespawnState::NONE:
	default:
		// If Found, keep TimeLeft at 0.0f
		// [TODO] If Spectate, game mode for this is not currently handled
		// [NOTE] Condition on being in this function is that we can't be in NONE state
		break;
	}
}
#pragma endregion // TIMER SYNC


#pragma region Client RPC
void AHISPlayerController::ClientUpdateHUDScores_Implementation(int SlotNumber, int NewScore)
{
	if (HISHUD && HISHUD->CharacterOverlay)
	{
		HISHUD->CharacterOverlay->UpdateScore(SlotNumber, NewScore);
	}
}
#pragma endregion // CLIENT RPC


#pragma region Match State
void AHISPlayerController::ServerCheckMatchState_Implementation()
{
	AHISGameMode* HISGameMode = Cast<AHISGameMode>(UGameplayStatics::GetGameMode(this));
	HISGameInstance = (HISGameInstance == nullptr) ? Cast<UHISGameInstance>(GetGameInstance()) : HISGameInstance;
	if (HISGameMode && HISGameInstance)
	{
		MatchState = HISGameMode->GetMatchState();
		PreGameWaitTime = HISGameMode->GameStartWaitTime;
		LevelStartingTime = HISGameMode->LevelStartingTime;

		GameTimeLimit = HISGameInstance->GetGameTimeLimit() * 60;			// Multiply by 60 as measured in minutes [TODO] Include the multiply in GetGameTimeLimit() ???
		HideTimeLimit = HISGameInstance->GetHideTimeLimit();
		RespawnWaitTime = HISGameInstance->GetRespawnTime();

		bIsGameTimeEndless = (GameTimeLimit == 0);
		bIsHideTimeEndless = (HideTimeLimit == 0);
		bNoRespawnTime = (RespawnWaitTime == 0);
		
		ClientJoinMidgame
		(
			MatchState, PreGameWaitTime, LevelStartingTime,
			GameTimeLimit, HideTimeLimit, RespawnWaitTime
		);
	}
}

void AHISPlayerController::ClientJoinMidgame_Implementation(FName State, float WaitTime, float StartingTime, float GameTime, float HideTime, float RespawnTime)
{
	MatchState = State;
	PreGameWaitTime = WaitTime;
	LevelStartingTime = StartingTime;
	GameTimeLimit = GameTime;
	HideTimeLimit = HideTime;
	RespawnWaitTime = RespawnTime;

	bIsGameTimeEndless = (GameTimeLimit == 0);
	bIsHideTimeEndless = (HideTimeLimit == 0);
	bNoRespawnTime = (RespawnWaitTime == 0);

	OnMatchStateSet(MatchState);
	if (HISHUD && MatchState == MatchState::WaitingToStart)
	{
		// Must call this in BeginPlay rather than checking for 
		// MatchState == WaitingToStart inside OnMatchStateSet()
		// as HUD won't exist at that point
		HISHUD->AddAnnouncementWidget();
	}
}

void AHISPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AHISPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AHISPlayerController::HandleMatchHasStarted()
{
	HISHUD = (HISHUD == nullptr) ? Cast<AHISHUD>(GetHUD()) : HISHUD;
	if (HISHUD)
	{
		HISHUD->AddCharacterOverlay();
		if (HISHUD->AnnouncementWidget)
		{
			// Set to hidden, rather than remove from viewport, as will want this for post-game as well as pre-game!
			HISHUD->AnnouncementWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	OnRespawnStateSet
	(
		(bNoRespawnTime) ? ERespawnState::Hider : ERespawnState::RespawnHiderWait
	);
}
#pragma endregion // MATCH STATE


#pragma region Respawn State
void AHISPlayerController::OnRespawnStateSet(ERespawnState State)
{
	RespawnState = State;
	HandleRespawnStateChanged();
}

void AHISPlayerController::OnRep_RespawnState()
{
	HandleRespawnStateChanged();
}

void AHISPlayerController::HandleRespawnStateChanged()
{
	AHISCharacter* HISCharacter = Cast<AHISCharacter>(GetCharacter());
	if (HISCharacter == nullptr) return;

	switch (RespawnState)
	{
	case ERespawnState::Found:
		HISCharacter->bDisableInput = true;
		//SetFoundTextVisible(true);
		if (HISHUD && HISHUD->CharacterOverlay) HISHUD->CharacterOverlay->SetFoundTextVisible(true);
		SetHUDTimerLabel(ETimerLabelState::WAIT);
		// [TODO][IMPORTANT] Temp below using countdown here! Want to trigger on found animation finished!
		SoloTimerStartTime = GetServerTime();
		SoloTimerLimit = 3.0f;
		break;

	case ERespawnState::RespawnHiderWait:
	case ERespawnState::RespawnSeekerWait:
		if (RespawnState == ERespawnState::RespawnHiderWait)
		{
			//SetFoundTextVisible(false);
			if (HISHUD && HISHUD->CharacterOverlay) HISHUD->CharacterOverlay->SetFoundTextVisible(false);
			SpawnCharacter(HiderClass, true);
		}
		else
		{
			SpawnClone();
			SpawnCharacter(SeekerClass, true);
		}
		SoloTimerStartTime = GetServerTime();
		SoloTimerLimit = RespawnWaitTime;
		SetHUDTimerLabel(ETimerLabelState::WAIT);
		break;

	case ERespawnState::Hider:
		if (bNoRespawnTime)
		{
			//SetFoundTextVisible(false);
			if (HISHUD && HISHUD->CharacterOverlay) HISHUD->CharacterOverlay->SetFoundTextVisible(false);
			SpawnCharacter(HiderClass, false);
		}
		else
		{
			HISCharacter->bDisableInput = false;
		}
		SoloTimerStartTime = GetServerTime();
		SoloTimerLimit = HideTimeLimit;
		SetHUDTimerLabel(ETimerLabelState::HIDE);
		break;

	case ERespawnState::Spectate:
		// NOT YET IMPLEMENTED
		//SetHUDTimerLabel(ETimerLabelState::VIEW);
		break;

	case ERespawnState::NONE:
		// Seeking
		if (bGameTimerStarted == false)
		{
			GameTimerStartingTime = GetServerTime();
			bGameTimerStarted = true;
		}
		if (bNoRespawnTime)
		{
			// [TODO][Q] Do we want pressing hide to:
			//	- Be a one time thing
			//	- Take the last place it was pressed
			//	- Un-hide upon being pressed a second time, then have to press to re-hide
			//		(so if you run out of time it'll take your current position)
			SpawnClone();
			SpawnCharacter(SeekerClass, false);
		}
		else
		{
			HISCharacter->bDisableInput = false;
		}
		SetHUDTimerLabel(ETimerLabelState::SEEK);
		break;

	default:
		break;
	}
}
#pragma endregion // RESPAWN STATE


#pragma region Positions
// IMPLEMENT PROPERLY!

/*
void AHISPlayerController::SetSpawnPosition(FVector Location, FRotator Rotation)
{
	if (HasAuthority() == false)
	{
		ServerSetSpawnPosition(Location, Rotation);
		return;
	}
	SpawnLocation = Location;
	SpawnRotation = Rotation;
}

void AHISPlayerController::ServerSetSpawnPosition_Implementation(FVector Location, FRotator Rotation)
{
	SpawnLocation = Location;
	SpawnRotation = Rotation;
}
//*/

void AHISPlayerController::SetHidePosition(FVector Location, FRotator Rotation)
{
	if (HasAuthority() == false)
	{
		ServerSetHidePosition(Location, Rotation);
		return;
	}

	HideLocation = Location;
	HideRotation = Rotation;
	bHidePositionSet = true;
	
	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (HISGameMode)
	{
		if (bGameTimerStarted)
			HISGameMode->SpawnHiddenClone(this);
		else
			HISGameMode->CheckAllClonesHidden();
	}
}

void AHISPlayerController::ServerSetHidePosition_Implementation(FVector Location, FRotator Rotation)
{
	HideLocation = Location;
	HideRotation = Rotation;
	bHidePositionSet = true;
	
	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (bGameTimerStarted)
	{
		HISGameMode->SpawnHiddenClone(this);
	}
	else
	{
		HISGameMode->CheckAllClonesHidden();
	}
}


void AHISPlayerController::SpawnCharacter(TSubclassOf<AHISCharacter> CharacterType, bool DisableInput)
{
	if (HasAuthority() == false) return;

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
	FVector PlayerStartLocation = PlayerStarts[Index]->GetTransform().GetLocation();
	FRotator PlayerStartRotation = PlayerStarts[Index]->GetTransform().GetRotation().Rotator();

	AActor* SpawnedCharacter = GetWorld()->SpawnActor(CharacterType, &PlayerStartLocation, &PlayerStartRotation);
	AHISCharacter* HISCharacterNew = Cast<AHISCharacter>(SpawnedCharacter);
	AHISCharacter* HISCharacterOld = Cast<AHISCharacter>(GetCharacter());

	if (HISCharacterNew)
	{
		Possess(HISCharacterNew);
		HISCharacterNew->bDisableInput = DisableInput;
		HISCharacterOld->Destroy();
	}
	else
	{
		UE_LOG(LogActor, Error, TEXT("[AHISPlayerController::SpawnCharacter] HISCharacterNew not yet initialised! Major issue! Possible race condition."));
	}
}

void AHISPlayerController::SpawnClone()
{
	if (HasAuthority() == false) return;

	FVector CloneLocation = (bGameTimerStarted == false && bHidePositionSet) ? HideLocation : GetCharacter()->GetTransform().GetLocation();
	FRotator CloneRotation = (bGameTimerStarted == false && bHidePositionSet) ? HideRotation : GetCharacter()->GetTransform().GetRotation().Rotator();
	AActor* SpawnedClone = GetWorld()->SpawnActor(CloneClass, &CloneLocation, &CloneRotation);
	AHISClone* Clone = Cast<AHISClone>(SpawnedClone);
	Clone->HISPlayerController = this;
}
#pragma endregion // POSITIONS
