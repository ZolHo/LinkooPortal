// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalDoor.h"
#include "GameFramework/GameModeBase.h"
#include "PortalCoreGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LINKOOPORTAL_API APortalCoreGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	APortalDoor* BlueDoor;
	
	UPROPERTY()
	APortalDoor* RedDoor;
};
