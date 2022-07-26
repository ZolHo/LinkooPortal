// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalHelperComponent.h"

#include "CanEnterPortal.h"
#include "LinkooTools.h"
#include "PortalDoor.h"
#include "PortalDoorManager.h"
#include "Kismet/GameplayStatics.h"
#include "LinkooPortal/LinkooPortalCharacter.h"

// Sets default values for this component's properties
UPortalHelperComponent::UPortalHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}


// Called when the game starts
void UPortalHelperComponent::BeginPlay()
{
	Super::BeginPlay();
	// 使用Delay让PDM先初始化完再获得它，故在InitialPDM结束前，是没有Overlap事件的
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &UPortalHelperComponent::InitialPDM, 1.0f, true);
	
}


// Called every frame
void UPortalHelperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* TempCopyItem;
	// tick复制品的Transform
	for (auto ActorItem : ActorsNearBlueDoor)
	{
		TempCopyItem = MasterServantMap[ActorItem];
		if (TempCopyItem) Cast<ICanEnterPortal>(ActorItem)->OnEnterPortalTick(PDM->BlueDoor.Get(), TempCopyItem);
	}
	for (auto ActorItem : ActorsNearRedDoor)
	{
		TempCopyItem = MasterServantMap[ActorItem];
		if (TempCopyItem) Cast<ICanEnterPortal>(ActorItem)->OnEnterPortalTick(PDM->RedDoor.Get(), TempCopyItem);
	}
}

void UPortalHelperComponent::SwitchMasterServant(AActor* MasterActor)
{

	AActor* ServantActor = MasterServantMap[MasterActor];
	
	Cast<ICanEnterPortal>(MasterActor)->OnSwitchMasterServant(ServantActor,this);
}

void UPortalHelperComponent::OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(CheckCanOverlap(OtherActor) && (!ActorsNearRedDoor.Find(OtherActor)) && (!ActorsNearBlueDoor.Find(OtherActor)) )
	{
		Cast<ICanEnterPortal>(OtherActor)->OnOuterOverlapBegin(OverlappedComponent, this);
	}
}


void UPortalHelperComponent::OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CheckCanOverlap(OtherActor))
	{
		Cast<ICanEnterPortal>(OtherActor)->OnOuterOverlapEnd(OverlappedComponent, this);

	}
}

void UPortalHelperComponent::OnInnerOvrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CheckCanOverlap(OtherActor))
	{
		Cast<ICanEnterPortal>(OtherActor)->OnInnerOverlapBegin(OverlappedComponent, this);

	}
}



void UPortalHelperComponent::OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CheckCanOverlap(OtherActor))
	{
		Cast<ICanEnterPortal>(OtherActor)->OnInnerOverlapEnd(OverlappedComponent, this);
	}
}

bool UPortalHelperComponent::CheckCanOverlap(AActor* TestActor)
{
	return TestActor->GetClass()->ImplementsInterface(UCanEnterPortal::StaticClass())
		&& !IsCopyActor(TestActor);
}

void UPortalHelperComponent::InitialPDM()
{
	// 获取Owner保存的PDM
	PDM = Cast<APortalDoorManager> (this->GetOwner());
	
	PDM->BlueDoor->DoorCollision->OnComponentBeginOverlap.AddDynamic(this,&UPortalHelperComponent::OnOuterOverlapBegin);
	PDM->RedDoor->DoorCollision->OnComponentBeginOverlap.AddDynamic(this,&UPortalHelperComponent::OnOuterOverlapBegin);
	
	PDM->BlueDoor->DoorCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnOuterOverlapEnd);
	PDM->RedDoor->DoorCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnOuterOverlapEnd);
	
	PDM->BlueDoor->InnerCollision->OnComponentBeginOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOvrlapBegin);
	PDM->RedDoor->InnerCollision->OnComponentBeginOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOvrlapBegin);
	
	PDM->BlueDoor->InnerCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOverlapEnd);
	PDM->RedDoor->InnerCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOverlapEnd);

	// 清除定时器
	GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
}
