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
	TArray<AActor*> ActorsNearDoor;

	// 保存自己的Owner Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<class APortalDoorManager> PDM;

public:

	UFUNCTION()
	virtual void OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnInnerOvrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
