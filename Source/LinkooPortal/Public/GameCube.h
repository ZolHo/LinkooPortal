// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CanBeGrab.h"
#include "CanEnterPortal.h"
#include "GameFramework/Actor.h"
#include "GameCube.generated.h"

UCLASS()
class LINKOOPORTAL_API AGameCube : public AActor, public ICanBeGrab, public ICanEnterPortal
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameCube();

	virtual AActor* SpawnCopyActor() override;

	virtual void OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent,UPortalHelperComponent* PortalHelper) override;
	
	virtual void OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper) override;
	
	virtual void OnInnerOverlapBegin(UPrimitiveComponent* OverlappedComponent,  UPortalHelperComponent* PortalHelper) override;
	
	virtual void OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper) override;

	virtual void OnEnterPortalTick(APortalDoor* NearDoor, AActor* CopyActor) override;

	virtual void OnSwitchMasterServant(AActor* CopyActor, UPortalHelperComponent* PortalHelper) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* MeshMaterial;

	

};
