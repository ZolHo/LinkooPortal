// Copyright Epic Games, Inc. All Rights Reserved.

#include "LinkooPortalGameMode.h"
#include "LinkooPortalHUD.h"
#include "LinkooPortalCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALinkooPortalGameMode::ALinkooPortalGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALinkooPortalHUD::StaticClass();

}
