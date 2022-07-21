// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalHelperComponent.generated.h"

// 负责传送，复制等操作
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINKOOPORTAL_API UPortalHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPortalHelperComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	// 用主Actor查找复制的Actor
	TMap<AActor*, AActor*> MasterServantMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	// 在传送门附近的可以传送的Actor需要创建复制并Tick
	TSet<AActor*> ActorsNearBlueDoor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<AActor*> ActorsNearRedDoor;

	TSet<AActor*> AllCopyActors;

	// 保存自己的Owner Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<class APortalDoorManager> PDM;

public:
	FTimerHandle DelayTimerHandle;
	void InitialPDM();

	UFUNCTION()
	// 切换主从Actor的位置
	void SwitchMasterServant(AActor* MasterActor);

	// 看看是不是复制的Actor
	inline bool IsCopyActor(AActor* TestActor)
	{
		return AllCopyActors.Find(TestActor) != nullptr;
	}

	// 触发Overlap的必备条件：实现接口， 不是复制的Actor
	bool CheckCanOverlap(AActor* TestActor);

	UFUNCTION()
	// 外门重叠，开始复制物体
	virtual void OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	// 外门结束重叠，停止复制
	virtual void OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	// 内门重叠，控制碰撞
	virtual void OnInnerOvrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	// 内门结束重叠，恢复碰撞，进行传送
	virtual void OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
