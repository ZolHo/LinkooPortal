// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PortalDoor.generated.h"

UENUM()
// 传送门种类
enum EPortalDoorType { Red,Blue };


UCLASS()
class LINKOOPORTAL_API APortalDoor : public AActor
{
	GENERATED_BODY()
	
	friend class APortalDoorManager;
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Mesh)
	UMaterial* DoorFaceMaterial;

	// 碰撞检测盒
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Collision)
	UBoxComponent* DoorCollision;

	// 触发传送用的包围盒
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Collision)
	UBoxComponent* InnerCollision;
	
	// 捕获的场景
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=View)
	USceneCaptureComponent2D* PortalViewCapture;

	// 保存门后面的对象，用于处理物理碰撞
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<class AActor> ActorWhichDoorStick;
	
public:

	// 获取另一扇传送门，可能返回null
	UFUNCTION(BlueprintCallable)
	APortalDoor* GetTheOtherPortal();

	// 获取传送门是否显示在游戏中的Active状态
	UFUNCTION(BlueprintCallable)
	bool IsActive() const;

	// 传送门在游戏中不销毁，而是切换Active状态隐藏起来
	UFUNCTION(BlueprintCallable)
	void SetDoorActive(bool state);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// 获取对应的另一扇传送门
	TWeakObjectPtr<APortalDoor> TheOtherDoor;

	// 激活状态，true为在游戏中显示， false反之
	bool ActiveState = true;
};