// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "PortalDoor.generated.h"

UENUM()
// 传送门种类
enum EPortalDoorType { Red,Blue };


UCLASS()
class LINKOOPORTAL_API APortalDoor : public AActor
{
	GENERATED_BODY()
	
	friend class FPortalDoorManager;
	
public:	
	// Sets default values for this actor's properties
	APortalDoor();

	// 红门或蓝门
	EPortalDoorType DoorType;

	// 门框
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Mesh)
	UStaticMeshComponent* DoorFrameMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Materia)
	UMaterialInterface* DoorFrameMaterialBlue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Materia)
	UMaterialInterface* DoorFrameMaterialRed;
	
	// 门面
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Mesh)
	UStaticMeshComponent* DoorFaceMesh;

	// 碰撞检测盒
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Collision)
	UBoxComponent* DoorCollision;

	// 捕获的场景
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=View)
	USceneCaptureComponent2D* PortalViewCapture;

public:

	// 获取另一扇传送门，可能返回null
	const APortalDoor* GetTheOtherPortal();

	// 获取传送门是否显示在游戏中的Active状态
	bool GetDoorActive() const;

	// 传送门在游戏中不销毁，而是切换Active状态隐藏起来
	void SetDoorActive(bool state);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 延迟构造时候的初始化
	void InitDoor(EPortalDoorType dtype);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// 获取对应的另一扇传送门
	APortalDoor* TheOtherDoor;

	// 激活状态，true为在游戏中显示， false反之
	bool ActiveState = true;
};


// 门管
class FPortalDoorManager
{
public:
	APortalDoor* BlueDoor;
	APortalDoor* RedDoor;

public:
	// 生成传送门，如果内存中存在，则用激活它代替生成
	bool SpawnOrActiveDoor(EPortalDoorType dtype, FTransform* spawnTransform, AActor* const caller);
	
	static FPortalDoorManager& Get();

	// 更新场景捕获组件的位置
	void UpdateViewTarget();
private:
	UWorld* NowWorld;

	// 清除Manager管理的门，因为Manager的生命周期大于World，所有切换world时候需要reset
	void ResetManager();
};