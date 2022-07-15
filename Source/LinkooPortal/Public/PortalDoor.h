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

	// 门面
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Mesh)
	UStaticMeshComponent* DoorFaceMesh;

	// 碰撞检测盒
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Collision)
	UBoxComponent* DoorCollision;

	// 显示另一个传送门中摄像机的场景
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=View)
	USceneCaptureComponent2D* PortalView;

	// 摄像机，渲染到另一个传送门上
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=View)
	UCameraComponent* ViewCamera;
	

public:

	// 获取另一扇传送门，可能返回null
	const APortalDoor* GetTheOtherPortal();

	bool GetDoorActive() const;

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
	
	APortalDoor* TheOtherDoor;
	
	bool ActiveState = true;
};


class FPortalDoorManager
{
public:
	APortalDoor* BlueDoor;
	APortalDoor* RedDoor;

public:
	// 生成传送门，如果内存中存在，则用激活它代替生成
	bool SpawnOrActiveDoor(EPortalDoorType dtype, FTransform* spawnTransform, AActor* const caller);
	
	static FPortalDoorManager& Get();
private:
	UWorld* NowWorld;

	void ResetManager();
};