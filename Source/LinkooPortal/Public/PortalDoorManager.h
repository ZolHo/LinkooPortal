// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalDoor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "LinkooPortal/LinkooPortalCharacter.h"
#include "PortalDoorManager.generated.h"

UCLASS()
class LINKOOPORTAL_API APortalDoorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalDoorManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APortalDoor* BlueDoor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APortalDoor* RedDoor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UChildActorComponent* BlueDoorComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UChildActorComponent* RedDoorComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTextureRenderTarget2D* TargetBlue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTextureRenderTarget2D* TargetRed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInterface* MateriaBlue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInterface* MateriaRed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Materia)
	UMaterialInterface* DoorFrameMaterialBlue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Materia)
	UMaterialInterface* DoorFrameMaterialRed;

	UPROPERTY()
	ALinkooPortalCharacter* Player;

public:
	// 生成传送门，如果内存中存在，则用激活它代替生成
	UFUNCTION(BlueprintCallable)
	bool SpawnOrActiveDoor(EPortalDoorType dtype, FTransform& spawnTransform);

	// 更新场景捕获组件的位置
	UFUNCTION(BlueprintCallable)
	void UpdateViewTarget();

	// 返回是否两个传送门都是激活状态
	UFUNCTION(BlueprintCallable)
	bool IsAllReady();
};
