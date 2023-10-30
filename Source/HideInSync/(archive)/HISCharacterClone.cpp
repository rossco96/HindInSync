// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacterClone.h"
#include "HISCharacterActive.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"										// [TODO] Can delete? Check!
#include "HideInSync/GameMode/HISGameMode.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
AHISCharacterClone::AHISCharacterClone()
{
	// Can probably delete the below, since inheriting from HISCharacter?
	PrimaryActorTick.bCanEverTick = true;

	// Don't think I need this below, as class inherits ACharacter (not APawn)
	// Would only be used for the AreaSphere code... Test it, I guess
	//bReplicates = true;

	// This is also not needed. Apparently. Again, think ACharacter already specifies? Somehow??
	//SetRootComponent(GetMesh());

	// [TODO] Only here & false temporarily!
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Create mesh subobject so players can be found
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Setup FoundWidget (to popup when clone can be tagged)
	FoundWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FoundWidget"));
	FoundWidget->SetupAttachment(RootComponent);
}


void AHISCharacterClone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Hide", IE_Pressed, this, &AHISCharacterClone::HideClone);
}


void AHISCharacterClone::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::BeginPlay] testy mctestface"));

	if (HasAuthority())
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::BeginPlay] HAS AUTHORITY (i.e. we are the server) SETTING AreaSphere THINGS."));

		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		// [TODO] Will we want to (and can we??) change ECollisionChannel to e.g. ECC_EnemyClone? Is this customisable??

		// Only want checks to be made on the server, so can assign here rather than the constructor:
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AHISCharacterClone::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AHISCharacterClone::OnSphereEndOverlap);



		/*
		AActor* PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(GetController());
		FString PlayerStartName = PlayerStart->GetName();
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::BeginPlay] PlayerStart1a... '%s'"), *PlayerStartName);
		FVector Location = PlayerStart->GetTransform().GetLocation();
		FString LocationString = Location.ToString();
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::BeginPlay] PlayerStart1b... '%s'"), *LocationString);

		AActor* PlayerStart2 = GetWorld()->GetAuthGameMode()->ChoosePlayerStart(GetController());
		FString PlayerStartName2 = PlayerStart2->GetName();
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::BeginPlay] PlayerStart2a... '%s'"), *PlayerStartName2);
		FVector Location2 = PlayerStart2->GetTransform().GetLocation();
		FString LocationString2 = Location2.ToString();
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::BeginPlay] PlayerStart2b... '%s'"), *LocationString2);
		//*/



		SpawnLocation = GetTransform().GetLocation();
		SpawnRotation = GetTransform().GetRotation().Rotator();
	}

	if (FoundWidget)
	{
		FoundWidget->SetVisibility(false);
	}
}


void AHISCharacterClone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion


#pragma region Input
void AHISCharacterClone::HideClone()
{
	if (HasAuthority())
	{
		bOnlyRelevantToOwner = false;
	}
	else
	{
		ServerHideButtonPressed();
	}



	// [TODO][12.09.2023]
	//		o Get GameMode
	//		o Pass current position as hiding place
	//		o (CREATE NEW TYPE OF ACTOR - NOT CONTROLLED, CHARACTER SHELL, CONTAINS DATA OF LINKED PLAYER)
	//		o Show owner of THIS either a static bird's eye view of the map, or a free cam (unable to see other clones, obviously)
	//		o Wait until either all players hidden or hide timeout reached
	//		o GameMode then responsible for:
	//			- Spawn CLONE at the location each player chose
	//			- Respawn each player at original (randomly chosen) location, this time with ACTIVE character
	//			- Countdown until level start, and then start the game

	// Clients don't have access to GameMode..? Will need this in HasAuthority() check?? Ensure HISGameMode != nullptr???
	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();



	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::HideClone]"));
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("HIDE CLONE!")));
	}
	// [TODO] Only need to set bOnlyRelevantToOwner to false?
	// Can have CollisionObjectType as ECC_WorldDynamic from start
	// ... And then what do about CollisionResponse Blocking?

	// [TODO] However! Want this executed by the GLM.
	// (Depending on the game state! If we're re-hiding mid-level, if settings permit, then want to execute it here... Right? Should still run via GLM? With timeout??)

	
	
	// RPC THIS!!!
	//bOnlyRelevantToOwner = false;



	// [TODO] Spawn HISCharacterSeeker from EXISTING (also [TODO]) PlayerStart position
	// Same rules as above, spawning via GLM (etc.)



	//GetWorld()->SpawnPlayActor();
	
	//AActor* SpawnedActor = GetWorld()->SpawnActor(AHISCharacterActive::StaticClass(), &SpawnLocation, &SpawnRotation);
	AActor* SpawnedActor = GetWorld()->SpawnActor(ActiveCharacterBP, &SpawnLocation, &SpawnRotation);

	// [TODO][IMPORTANT] Need all this interaction to be done within InLevelGameMode!!! 
	// i.e. Make call to Spawn the ActiveCharacter in there, and transfer ownership in there

	AHISCharacter* ActiveCharacter = Cast<AHISCharacter>(SpawnedActor);
	//ActiveCharacter->Set
	// 
	//Controller->SetPawn(ActiveCharacter);
	//Controller = ActiveCharacter->Controller;

	//Controller->UnPossess();														// no
	//ActiveCharacter->Controller->Possess(ActiveCharacter);
}


void AHISCharacterClone::ServerHideButtonPressed_Implementation()
{
	// RPC THIS!!!
	bOnlyRelevantToOwner = false;
}
#pragma endregion


#pragma region Sphere Overlaps
void AHISCharacterClone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHISCharacterActive* Character = Cast<AHISCharacterActive>(OtherActor);
	FString ActorType = OtherActor->GetName();
	if (Character != nullptr)
	{
		Character->SetFoundClone(this);
	}
}


void AHISCharacterClone::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AHISCharacterActive* Character = Cast<AHISCharacterActive>(OtherActor);
	if (Character)
	{
		Character->SetFoundClone(nullptr);
	}
}
#pragma endregion


AHISPlayerController* AHISCharacterClone::GetHISPlayerController()
{
	return Cast<AHISPlayerController>(Controller);
}


void AHISCharacterClone::ShowFoundWidget(bool bShowWidget)
{
	if (FoundWidget)
	{
		FoundWidget->SetVisibility(bShowWidget);
	}
}


#pragma region FOUND Methods
void AHISCharacterClone::FoundIgnore()
{
	// [TODO]
	//	Do nothing to this player, other than perhaps a red vignette and sfx to the linked Active character
	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::FoundIgnore][TODO] -- Do nothing, other than 'found' sound and visual effects to the linked Active Character ONLY."));
}

void AHISCharacterClone::FoundReset_Implementation()
{
	bIsFound = true;

	// [TODO]
	//	o Show destroy animations + effects (+ sounds?)
	//	o Destroy
	//	o Respawn
	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::FoundReset][TODO] -- Show destroy animations and effects (and possibly sounds?) -- Destroy and respawn."));
}

void AHISCharacterClone::FoundSpectate()
{
	bIsFound = true;

	// [TODO]
	//	o Show destroy animations + effects (+ sounds?)
	//	o Destroy
	//	o Change to EITHER player-follow-cam(s) OR free-cam
	UE_LOG(LogActor, Warning, TEXT("[AHISCharacterClone::FoundSpectate][TODO] -- Show destroy animations and effects (and possibly sounds?) -- Destroy -- EITHER allow viewing 3rd person cams of all remaining players, OR allow free-cam mode."));
}
#pragma endregion
