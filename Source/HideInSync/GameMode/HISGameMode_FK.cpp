// Fill out your copyright notice in the Description page of Project Settings.


#include "HISGameMode_FK.h"
#include "HideInSync/Character/HISClone.h"
#include "HideInSync/PlayerController/HISPlayerController.h"
#include "HideInSync/Character/HISCharacter.h"

// GameMode_FK = Finders Keepers
//	o Player with the most finds at the end of the time limit wins
//	o Clones do disappear (for all clients) on being found for the first time
//	o Clone owners (the respective seekers) are frozen once found
//	o Clone owners are then reset as hiders (with their own hiding time limit, if applicable)

void AHISGameMode_FK::PlayerFound(class AHISClone* FoundClone, class AHISPlayerController* SeekerController)
{
	Super::PlayerFound(FoundClone, SeekerController);		// [TODO] Want this? Or override completely? Figure out if anything in common! E.g. scores
	
	if (FoundClone)
	{
		// [TODO] See bullet points...
		
		//	o Play FOUND animation and sound for clone (and disappear)		--> Put this inside "FoundReset"
		//	o Destroy the clone
		// IMPORTANT! Need to check HasAuthority() then either call FoundReset or ClientFoundReset <<< is that correct? client? in general??
		FoundClone->FoundReset();

		//	o Freeze found player's seeker (disable input)
		int FoundPlayerId = FoundClone->GetPlayerId();						// Do it like this? Or should I be passing HiderController to this function?
		AHISPlayerController* FoundPlayerController = PlayersData[FoundPlayerId].GetController();
		//FoundPlayerController->SetIgnorePlayerInput(true);				// No longer doing it like this -- DELETE
		AHISCharacter* HISCharacter = Cast<AHISCharacter>(FoundPlayerController->GetPawn());
		HISCharacter->bDisableInput = true;
		
		//	o Zoom out from seeker (move first person camera to third person)
		
		//	o Play FOUND animation and sound for seeker (and disappear, and destroy)
		
		//	o Respawn (after a few seconds?) the found player as a hider
		//RequestRespawn(FoundPlayerId);									// Commented out for now, to test client SetIgnorePlayerInput
	}


	// [TODO][Important]
	// Need to find a way of "Finding" the linked active player as well...
	//		- Do via FoundCharacter->Found() ?
	//		OR 
	//		- Do via HISGameMode (e.g. lookup the ActivePlayer corresponding to this FoundCharacter, somehow, and send message like that)
}
