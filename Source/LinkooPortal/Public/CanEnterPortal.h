// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalDoor.h"
#include "PortalHelperComponent.h"
#include "UObject/Interface.h"
#include "CanEnterPortal.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCanEnterPortal : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LINKOOPORTAL_API ICanEnterPortal
{
	GENERATED_BODY()
public:
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	UFUNCTION()
	virtual AActor* SpawnCopyActor() = 0;

	virtual void OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)= 0;

	virtual void OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)= 0;

	virtual void OnInnerOverlapBegin(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)= 0;

	virtual void OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)= 0;

	virtual void OnEnterPortalTick(APortalDoor* NearDoor, AActor* CopyActor)= 0;

	virtual void OnSwitchMasterServant(AActor* CopyActor, UPortalHelperComponent* PortalHelper) = 0;
};
