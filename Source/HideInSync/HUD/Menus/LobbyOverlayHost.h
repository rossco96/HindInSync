// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyOverlayBase.h"
#include "LobbyOverlayHost.generated.h"

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API ULobbyOverlayHost : public ULobbyOverlayBase
{
	GENERATED_BODY()

public:
	void SetButtonStartGameEnabled(bool Enabled);
	
private:
	// Need binding for enabling and disabling
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonStartGame;

	// Need binding to ensure the next player in the lobby can see a "becoming host" popup and receive the LobbyOverlayHost
	// (Make sure we show 'are you sure' prompt for both host and client)
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonBack;

	// Can just use blueprints for the below?
	// ButtonMatchSettings, ButtonCharacterCustomisation, ButtonPlayerSettings
};
