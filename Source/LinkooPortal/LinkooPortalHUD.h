// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LinkooPortalHUD.generated.h"

UCLASS()
class ALinkooPortalHUD : public AHUD
{
	GENERATED_BODY()

public:
	ALinkooPortalHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

