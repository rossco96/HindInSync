// Fill out your copyright notice in the Description page of Project Settings.


#include "HISGameMode_FK.h"
#include "HideInSync/Character/HISClone.h"
#include "HideInSync/PlayerController/HISPlayerController.h"

// GameMode_FK = Finders Keepers
//	o Player with the most finds at the end of the time limit wins
//	o Clones do disappear (for all clients) on being found for the first time
//	o Clone owners (the respective seekers) are frozen once found
//	o Clone owners are then reset as hiders (with their own hiding time limit, if applicable)

void AHISGameMode_FK::PlayerFound(class AHISClone* FoundClone, /*class AHISPlayerController* HiderController,*/ class AHISPlayerController* SeekerController)
{
	Super::PlayerFound(FoundClone, /*HiderController,*/ SeekerController);		// [TODO] Want this? Or override completely? Figure out if anything in common! E.g. scores
	
	if (FoundClone)
	{
		FoundClone->FoundReset();

		int FoundPlayerId = FoundClone->GetPlayerId();
		AHISPlayerController* FoundPlayerController = PlayersData[FoundPlayerId].GetController();
		FoundPlayerController->FreezePlayer();

		// [TODO]
		//	o Play FOUND animation and sound for clone (and disappear)
		//	o Freeze found seeker (disable input)
		//	o Zoom out from seeker (move first person camera to third person)
		//	o Play FOUND animation and sound for seeker (and disappear)
		//	o Respawn (after a few seconds?) the found player as a hider
	}

	// [TODO][Important]
	// Need to find a way of "Finding" the linked active player as well...
	//		- Do via FoundCharacter->Found() ?
	//		OR 
	//		- Do via HISGameMode (e.g. lookup the ActivePlayer corresponding to this FoundCharacter, somehow, and send message like that)
}
