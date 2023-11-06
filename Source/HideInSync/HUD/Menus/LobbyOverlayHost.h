// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyOverlayBase.h"
#include "LobbyOverlayHost.generated.h"

// [TODO]
// Refactor this! Want LobbyOverlayBase, with children Host and Client
// (and what about Public lobbies? Same as private Client?)

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API ULobbyOverlayHost : public ULobbyOverlayBase
{
	GENERATED_BODY()

public:
	//class UButton* ButtonStartGame;
	void SetButtonStartGameEnabled(bool Enabled);
	
private:
	UPROPERTY(meta = (BindWidget))				// Should this be		UPROPERTY(meta = (BindWidget))	or	UPROPERTY(EditAnywhere)		??? Enabling/disabling as people join/leave the lobby
	class UButton* ButtonStartGame;

	// Should I just make ButtonStartGame public? Only really used for setting enabled/disabled?

	// Do the below need to be declared in this class? Or can it be done entirely through blueprints?

	/*
	UPROPERTY(EditAnywhere)
	UButton* ButtonMatchSettings;

	UPROPERTY(EditAnywhere)
	UButton* ButtonCharacterCustomisation;

	UPROPERTY(EditAnywhere)
	UButton* ButtonPlayerSettings;

	UPROPERTY(EditAnywhere)
	UButton* ButtonBack;
	//*/
};
