// Fill out your copyright notice in the Description page of Project Settings.


#include "HISClone.h"
#include "HISCharacterSeeker.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
AHISClone::AHISClone()
{
	bReplicates = true;

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	SetRootComponent(AreaSphere);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CloneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CloneMesh->SetupAttachment(AreaSphere);

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollider"));
	CapsuleCollider->SetupAttachment(CloneMesh);

	FoundWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FoundWidget"));
	FoundWidget->SetupAttachment(AreaSphere);
}


void AHISClone::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		// Only want checks to be made on the server, so can assign here rather than the constructor:
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AHISClone::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AHISClone::OnSphereEndOverlap);
	}

	if (FoundWidget)
	{
		FoundWidget->SetVisibility(false);
	}
}


void AHISClone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AHISClone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHISClone, PlayerId);
}
#pragma endregion


#pragma region Sphere Overlaps
void AHISClone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHISCharacterSeeker* Character = Cast<AHISCharacterSeeker>(OtherActor);
	FString ActorType = OtherActor->GetName();
	if (Character != nullptr)
	{
		Character->SetFoundClone(this);
	}
}


void AHISClone::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AHISCharacterSeeker* Character = Cast<AHISCharacterSeeker>(OtherActor);
	if (Character)
	{
		Character->SetFoundClone(nullptr);
	}
}
#pragma endregion


void AHISClone::ShowFoundWidget(bool bShowWidget)
{
	if (FoundWidget)
	{
		FoundWidget->SetVisibility(bShowWidget);
	}
}


#pragma region FOUND Methods
void AHISClone::FoundIgnore()
{
	// [TODO]
	//	Do nothing to this player, other than perhaps a red vignette and sfx to the linked Active character
	UE_LOG(LogActor, Warning, TEXT("[AHISClone::FoundIgnore][TODO] -- Do nothing, other than 'found' sound and visual effects to the linked Active Character ONLY."));
}

void AHISClone::FoundReset_Implementation()
{
	//bIsFound = true;										// Do we need this?

	// [TODO]
	
	//	o Show destroy animations + effects (+ sounds?)
	
	//	o Destroy
	Destroy();
	
	//	o Respawn (handled outside of this function)

	//UE_LOG(LogActor, Warning, TEXT("[AHISClone::FoundReset][TODO] -- Show destroy animations and effects (and possibly sounds?) -- Destroy and respawn."));
}

void AHISClone::FoundSpectate()
{
	//bIsFound = true;

	// [TODO]
	//	o Show destroy animations + effects (+ sounds?)
	//	o Destroy
	//	o Change to EITHER player-follow-cam(s) OR free-cam
	UE_LOG(LogActor, Warning, TEXT("[AHISClone::FoundSpectate][TODO] -- Show destroy animations and effects (and possibly sounds?) -- Destroy -- EITHER allow viewing 3rd person cams of all remaining players, OR allow free-cam mode."));
}
#pragma endregion
