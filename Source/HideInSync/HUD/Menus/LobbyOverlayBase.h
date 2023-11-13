// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyOverlayBase.generated.h"

// [TODO][Important]
// Need to conisder what 
// Everyone receive LobbyHostClient? Or do we need any differences, and create a LobbyOverlayPublic class?
// (perhaps a voting system for map and settings? Or do you choose those beforehand and hope to match with people ... if so, have limited options for a public lobby)

/**
 * 
 */
UCLASS()
class HIDEINSYNC_API ULobbyOverlayBase : public UUserWidget
{
	GENERATED_BODY()
	
};
