// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalHelperComponent.h"

#include "CanEnterPortal.h"
#include "LinkooTools.h"
#include "PortalDoor.h"
#include "PortalDoorManager.h"

// Sets default values for this component's properties
UPortalHelperComponent::UPortalHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPortalHelperComponent::BeginPlay()
{
	Super::BeginPlay();
	// 使用Delay让PDM先初始化完再获得它，故在InitialPDM结束前，是没有Overlap事件的
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &UPortalHelperComponent::InitialPDM, 1.0f, true);
	// InitialPDM();
}


// Called every frame
void UPortalHelperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* TempCopyItem;
	for (auto ActorItem : ActorsNearBlueDoor)
	{
		TempCopyItem = MasterServantMap[ActorItem];
		TempCopyItem->SetActorTransform(ULinkooTools::CaculTransformForPortal(FTransform(UKismetMathLibrary::MakeRotFromXZ(ULinkooTools::CaculReflectVector(ActorItem->GetActorForwardVector(), PDM->BlueDoor->GetActorForwardVector()), ULinkooTools::CaculReflectVector(ActorItem->GetActorUpVector(), PDM->BlueDoor->GetActorForwardVector())), ULinkooTools::CaculReflectLocation(ActorItem->GetActorLocation(), PDM->BlueDoor->GetActorLocation(), PDM->BlueDoor->GetActorForwardVector())),PDM->BlueDoor->GetTransform(), PDM->RedDoor->GetTransform()));
	}
	for (auto ActorItem : ActorsNearRedDoor)
	{
		TempCopyItem = MasterServantMap[ActorItem];
		TempCopyItem->SetActorTransform(ULinkooTools::CaculTransformForPortal(FTransform(UKismetMathLibrary::MakeRotFromXZ(ULinkooTools::CaculReflectVector(ActorItem->GetActorForwardVector(), PDM->RedDoor->GetActorForwardVector()), ULinkooTools::CaculReflectVector(ActorItem->GetActorUpVector(), PDM->BlueDoor->GetActorForwardVector())), ULinkooTools::CaculReflectLocation(ActorItem->GetActorLocation(), PDM->RedDoor->GetActorLocation(), PDM->RedDoor->GetActorForwardVector())),PDM->RedDoor->GetTransform(), PDM->BlueDoor->GetTransform()));
	}
}

void UPortalHelperComponent::OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(Cast<ICanEnterPortal> (OtherActor) && (!AllNearActors.Find(OtherActor)) && (!AllCopyActors.Find(OtherActor)))
	{
		AActor** ServantActorPtr = MasterServantMap.Find(OtherActor);
		if (ServantActorPtr)
		{
			(*ServantActorPtr)->SetActorHiddenInGame(false);
		}
		else
		{
			AActor* ServantActor = GetWorld()->SpawnActor(OtherActor->GetClass());
			AllCopyActors.Add(ServantActor);
			auto CollisionComponent = ServantActor->FindComponentByClass<UPrimitiveComponent>();
			CollisionComponent->IgnoreActorWhenMoving(PDM->BlueDoor->ActorWhichDoorStick.Get(), true);
			CollisionComponent->IgnoreActorWhenMoving(PDM->RedDoor->ActorWhichDoorStick.Get(), true);
			MasterServantMap.Add(OtherActor, ServantActor);
		}

		// 将Actor加入Array和Set引用
		if (OverlappedComponent->GetOwner() == PDM->BlueDoor)
		{
			ActorsNearBlueDoor.AddUnique(OtherActor);
		}
		else
		{
			ActorsNearRedDoor.AddUnique(OtherActor);
		}
		AllNearActors.Add(OtherActor);
	}
}


void UPortalHelperComponent::OnInnerOvrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void UPortalHelperComponent::OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void UPortalHelperComponent::OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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
