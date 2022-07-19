// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalHelperComponent.h"

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

	// 获取Owner保存
	PDM = Cast<APortalDoorManager> (this->GetOwner());
	
	PDM->BlueDoor->DoorCollision->OnComponentBeginOverlap.AddDynamic(this,&UPortalHelperComponent::OnOuterOverlapBegin);
	PDM->RedDoor->DoorCollision->OnComponentBeginOverlap.AddDynamic(this,&UPortalHelperComponent::OnOuterOverlapBegin);

	PDM->BlueDoor->DoorCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnOuterOverlapEnd);
	PDM->RedDoor->DoorCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnOuterOverlapEnd);

	PDM->BlueDoor->InnerCollision->OnComponentBeginOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOvrlapBegin);
	PDM->RedDoor->InnerCollision->OnComponentBeginOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOvrlapBegin);

	PDM->BlueDoor->InnerCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOverlapEnd);
	PDM->RedDoor->InnerCollision->OnComponentEndOverlap.AddDynamic(this, &UPortalHelperComponent::OnInnerOverlapEnd);
	
}


// Called every frame
void UPortalHelperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPortalHelperComponent::OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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
