// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CanBePaint.h"
#include "GameFramework/Actor.h"
#include "PortalWall.generated.h"

UCLASS()
class LINKOOPORTAL_API APortalWall : public AActor, public ICanBePaint
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalWall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* DefaultMaterial;

	bool bIsEnableOpenDoor = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
