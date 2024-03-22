// Fill out your copyright notice in the Description page of Project Settings.


#include "HISCharacterSeeker.h"
#include "Camera/CameraComponent.h"
#include "HISClone.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "HideInSync/GameMode/HISGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


#pragma region Unreal Functions
AHISCharacterSeeker::AHISCharacterSeeker()
{
	POVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	POVCamera->SetupAttachment(GetMesh());
	POVCamera->bUsePawnControlRotation = true;
}


void AHISCharacterSeeker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Find", IE_Pressed, this, &AHISCharacterSeeker::FindClone);
}


void AHISCharacterSeeker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (/*IsLocallyControlled() &&*/ ClonesInRange > 0)		// [TODO] Be careful with IsLocallyControlled! May need to do ClonesInRange++ or -- on the server and replicate?
	{
		RaycastViewport();
	}
}
#pragma endregion


#pragma region Input
void AHISCharacterSeeker::FindClone()
{
	if (FoundClone == nullptr) return;

	if (HasAuthority())
	{
		AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
		if (HISGameMode)
		{
			HISGameMode->PlayerFound(FoundClone, HISPlayerController);
		}
	}
	else
	{
		ServerFindButtonPressed();
	}
}
#pragma endregion


void AHISCharacterSeeker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AHISCharacterSeeker, FoundClone, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AHISCharacterSeeker, ClonesInRange, COND_OwnerOnly);
}


void AHISCharacterSeeker::OnRep_FoundClone(AHISClone* LastClone)
{
	if (HISPlayerController == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::OnRep_FoundClone] HISPlayerController is nullptr -- MAJOR ERROR AT THIS POINT !!!!!"));
		return;
	}

	//if (FoundClone && FoundClone->GetPlayerId() == HISPlayerController->GetPlayerId()) return;
	if (FoundClone && FoundClone->HISPlayerController == HISPlayerController) return;
	
	if (FoundClone)
	{
		FoundClone->ShowFoundWidget(true);
	}
	if (LastClone)
	{
		LastClone->ShowFoundWidget(false);
	}
}


void AHISCharacterSeeker::SetCloneInRange(AHISClone* Clone, bool bIsInRange)
{
	// [TODO][Important]
	// Here:
	//	o If a player hides at their own respawn location, this function is called and HISPlayerController is nullptr at this point
	// Some other bit of code:
	//	o If a player hides at another player's respawn location, the outline will not show up (they will have to move away and come back to find them)

	// Below is not ideal... See note above
	if (HISPlayerController == nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("[AHISCharacterSeeker::SetCloneInRange] HISPlayerController is nullptr"));
		return;
	}

	//if (Clone && Clone->GetPlayerId() == HISPlayerController->GetPlayerId()) return;
	if (Clone && Clone->HISPlayerController == HISPlayerController) return;

	if (bIsInRange)
	{
		ClonesInRange++;
	}
	else
	{
		ClonesInRange--;

		// [TODO] Figure out if want IsLocallyControlled here? Can it ever be true, or is this function only ever accessed via the server?
		if (/*IsLocallyControlled() &&*/ bFoundCloneVisible && Clone == FoundClone)
		{
			if (IsLocallyControlled()) Clone->ShowFoundWidget(false);
			FoundClone = nullptr;
			bFoundCloneVisible = false;
		}
	}
}


void AHISCharacterSeeker::RaycastViewport()
{
	FVector StartPos = POVCamera->GetComponentTransform().GetLocation();
	// [TODO] 160.0f is current radius of Clone's AreaSphere... Use that value though, so no need changing this value!!
	FVector EndPos = StartPos + (GetControlRotation().Vector() * 160.0f);

	
	// HISClone collisions:
	//	o AreaSphere		WorldDynamic			<<< why tho [TODO][IMPORTANT] Change the collision type of this!
	//	o CloneMesh			WorldStatic
	//	o CapsuleCollider	Pawn

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	// [????] ECC type required?
	// --> Nope, typeS required!
	// --> Need to have both Pawn and WorldStatic
	// --> Cannot have WorldDynamic since that's the AreaSphere type...
	// --> IMPORTANT NOTE: This means that any moving objects (e.g. doors) will need to be classed as WorldStatic for collision purposes
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	
	FHitResult HitResult;
	bool bSphereTraceHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		StartPos,
		EndPos,
		50.0f,																								// [TODO] This is currently arbitrary!
		TraceObjectTypes,
		false,
		{ this },
		IsLocallyControlled() ? EDrawDebugTrace::Type::None : EDrawDebugTrace::Type::ForDuration,			// [TODO] DEBUG! Change to None
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.5f
	);

	if (bSphereTraceHit)
	{
		AHISClone* Clone = Cast<AHISClone>(HitResult.GetActor());
		if (Clone)
		{
			//if (Clone->GetPlayerId() == HISPlayerController->GetPlayerId())
			if (Clone->HISPlayerController == HISPlayerController)
			{
				return;
			}

			if (bFoundCloneVisible == false)
			{
				if (IsLocallyControlled()) Clone->ShowFoundWidget(true);							// [TODO] THIS IS NOT IDEAL ??? (and more similar below) --- Is this mainly because of needing to draw the debug ray for testing?
				FoundClone = Clone;
				bFoundCloneVisible = true;
			}
			else if (FoundClone && FoundClone != Clone)
			{
				if (IsLocallyControlled()) FoundClone->ShowFoundWidget(false);
				if (IsLocallyControlled()) Clone->ShowFoundWidget(true);
				FoundClone = Clone;
			}
		}
		else if (FoundClone && bFoundCloneVisible)
		{
			if (IsLocallyControlled()) FoundClone->ShowFoundWidget(false);
			FoundClone = nullptr;
			bFoundCloneVisible = false;
		}
	}
	else
	{
		if (FoundClone && bFoundCloneVisible)
		{
			if (IsLocallyControlled()) FoundClone->ShowFoundWidget(false);
			FoundClone = nullptr;
			bFoundCloneVisible = false;
		}
	}



	// if the clone is in the list, then it is within range
	// if we're looking at the clone, then check if there's any objects in the way (raycast)
	// if not, and if the found outline is NOT already showing, then show it
	// (clone can be found! set corresponding bool to true!)

	// if the clone is removed from the list, stop showing its outline		-- clone cannot be found
	// if we're no longer looking at the clone, stop showing its outline	-- clone cannot be found
	// if something obstructs the clone (raycast), stop showing its outline	-- clone cannot be found
	// (^set any of the above corresponding bools to false)
}


#pragma region Server Functions
void AHISCharacterSeeker::ServerFindButtonPressed_Implementation()
{
	AHISGameMode* HISGameMode = GetWorld()->GetAuthGameMode<AHISGameMode>();
	if (HISGameMode)
	{
		HISGameMode->PlayerFound(FoundClone, HISPlayerController);
	}
}
#pragma endregion
