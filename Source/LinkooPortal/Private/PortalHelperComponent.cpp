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
	ActorsNearBlueDoor.Remove(MasterActor);
	ActorsNearRedDoor.Remove(MasterActor);
	
	check (MasterServantMap[MasterActor]);
	AActor* ServantActor = MasterServantMap[MasterActor];
	FTransform TempTransform =MasterActor->GetTransform();
	MasterActor->SetActorTransform(ServantActor->GetTransform());
	ServantActor->SetActorTransform(TempTransform);
}

void UPortalHelperComponent::OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(CheckCanOverlap(OtherActor) && (!ActorsNearRedDoor.Find(OtherActor)) && (!ActorsNearBlueDoor.Find(OtherActor)) )
	{
		Cast<ICanEnterPortal>(OtherActor)->OnOuterOverlapBegin(OverlappedComponent, this);
		// AActor** ServantActorPtr = MasterServantMap.Find(OtherActor);
		// if (ServantActorPtr)
		// {
		// 	(*ServantActorPtr)->SetActorHiddenInGame(false);
		// }
		// else
		// {
		// 	AActor* ServantActor =  Cast<ICanEnterPortal>(OtherActor)->SpawnCopyActor();
		//
		// 	AllCopyActors.Add(ServantActor);
		// 	MasterServantMap.Add(OtherActor, ServantActor);
		// }
		//
		// // 将Actor加入Array和Set引用
		// if (OverlappedComponent->GetOwner() == PDM->BlueDoor)
		// {
		// 	ActorsNearBlueDoor.Add(OtherActor);
		// }
		// else
		// {
		// 	ActorsNearRedDoor.Add(OtherActor);
		// }

	}
}


void UPortalHelperComponent::OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CheckCanOverlap(OtherActor))
	{
		Cast<ICanEnterPortal>(OtherActor)->OnOuterOverlapEnd(OverlappedComponent, this);
		// if (ULinkooTools::AIsFrontOfB(OtherActor, OverlappedComponent->GetOwner()))
		// {
		// 	// 正面出去则是正常出
		// 	ActorsNearRedDoor.Remove(OtherActor);
		// 	ActorsNearBlueDoor.Remove(OtherActor);
		// 	if(MasterServantMap[OtherActor]) MasterServantMap[OtherActor]->SetActorHiddenInGame(true);
		// }
		// else
		// {
		// 	// 从后面出去说明准备传送, 速度计算公式：RotatorB * Inv(RotatorA) * VelocityVector
		// 	// FVector Velocity = MasterServantMap[OtherActor]->GetActorRotation().RotateVector(OtherActor->GetActorRotation().UnrotateVector(OtherActor->GetVelocity()));
		// 	// float Vel = UKismetMathLibrary::Clamp(Velocity.Size(), 20.0f, 500.0f);
		// 	SwitchMasterServant(OtherActor);
		// 	OtherActor->FindComponentByClass<UPrimitiveComponent>()->SetPhysicsLinearVelocity(OtherActor->GetVelocity().Size()*OverlappedComponent->GetForwardVector());
		// }
	}
}

void UPortalHelperComponent::OnInnerOvrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CheckCanOverlap(OtherActor))
	{
		Cast<ICanEnterPortal>(OtherActor)->OnInnerOverlapBegin(OverlappedComponent, this);
		// if (ULinkooTools::AIsFrontOfB(OtherActor, OverlappedComponent->GetOwner()))
		// {
		// 	// 正面进入则取消碰撞，准备穿墙
		// 	auto ActorPrimitive = OtherActor->FindComponentByClass<UPrimitiveComponent>();
		// 	ActorPrimitive->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
		// }
	}
}



void UPortalHelperComponent::OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CheckCanOverlap(OtherActor))
	{
		Cast<ICanEnterPortal>(OtherActor)->OnInnerOverlapEnd(OverlappedComponent, this);
		// OtherActor->FindComponentByClass<UPrimitiveComponent>()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
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
