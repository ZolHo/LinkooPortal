// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalDoorManager.h"

#include "LinkooTools.h"
#include "PortalDoor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LinkooPortal/LinkooPortalCharacter.h"

// Sets default values
APortalDoorManager::APortalDoorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScence"));
	
	BlueDoorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("BlueDoor"));
	RedDoorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("RedDoor"));

	BlueDoorComponent->SetupAttachment(RootComponent);
	RedDoorComponent->SetupAttachment(RootComponent);

	PortalHelper = CreateDefaultSubobject<UPortalHelperComponent>(TEXT("PortalHelper"));
}

// Called when the game starts or when spawned
void APortalDoorManager::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<ALinkooPortalCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	
	BlueDoor = Cast<APortalDoor>(BlueDoorComponent->GetChildActor());
	RedDoor = Cast<APortalDoor>(RedDoorComponent->GetChildActor());
	BlueDoor->PortalViewCapture->bEnableClipPlane = true;
	RedDoor->PortalViewCapture->bEnableClipPlane = true;

	BlueDoor->DoorType = EPortalDoorType::Blue;
	RedDoor->DoorType = EPortalDoorType::Red;

	BlueDoor->DoorFrameMesh->SetMaterial(0, DoorFrameMaterialBlue);
	RedDoor->DoorFrameMesh->SetMaterial(0, DoorFrameMaterialRed);
	

	BlueDoor->PortalViewCapture->TextureTarget = TargetBlue;
	RedDoor->PortalViewCapture->TextureTarget = TargetRed;

	RedDoor->TheOtherDoor = BlueDoor;
	BlueDoor->TheOtherDoor = RedDoor;

	BlueDoor->SetDoorActive(false);
	RedDoor->SetDoorActive(false);
}

// Called every frame
void APortalDoorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsAllReady())
	{
		UpdateViewTarget();
	}
}

APortalDoor* APortalDoorManager::SpawnOrActiveDoor(EPortalDoorType dtype, FTransform& spawnTransform)
{
	// 利用指针的指针指向这次要操作的是红门还是蓝门
	TWeakObjectPtr<APortalDoor> dealDoor;
	if (dtype == EPortalDoorType::Blue) dealDoor = BlueDoor;
	else dealDoor = RedDoor;
	
	if(dealDoor.IsValid())
	{
		dealDoor->SetDoorActive(true);
		dealDoor->SetActorTransform(spawnTransform);
	}

	if (IsAllReady())
	{
		BlueDoor->DoorFaceMesh->SetMaterial(0, MateriaRed);
		RedDoor->DoorFaceMesh->SetMaterial(0, MateriaBlue);

		BlueDoor->SetActorEnableCollision(true);
		RedDoor->SetActorEnableCollision(true);
	}
	
	// TODO: 完善生成条件判断
	return dealDoor.Get();
}

void APortalDoorManager::UpdateViewTarget()
{
	auto PlayerCamera = Player->GetFirstPersonCameraComponent();
	FVector BPWL = ULinkooTools::CaculReflectLocation(PlayerCamera->GetComponentLocation(), RedDoor->GetActorLocation(), RedDoor->GetActorForwardVector());
	FVector RPWL = ULinkooTools::CaculReflectLocation(PlayerCamera->GetComponentLocation(), BlueDoor->GetActorLocation(), BlueDoor->GetActorForwardVector());
	
	FRotator BPWR = UKismetMathLibrary::MakeRotFromXZ(ULinkooTools::CaculReflectVector(PlayerCamera->GetForwardVector(), RedDoor->GetActorForwardVector()), ULinkooTools::CaculReflectVector(PlayerCamera->GetUpVector(), RedDoor->GetActorForwardVector()));
	FRotator RPWR = UKismetMathLibrary::MakeRotFromXZ(ULinkooTools::CaculReflectVector(PlayerCamera->GetForwardVector(), BlueDoor->GetActorForwardVector()), ULinkooTools::CaculReflectVector(PlayerCamera->GetUpVector(), BlueDoor->GetActorForwardVector()));

	FTransform TransBlue;
	TransBlue.SetLocation(BPWL);
	TransBlue.SetRotation(BPWR.Quaternion());

	FTransform TransRed;
	TransRed.SetLocation(RPWL);
	TransRed.SetRotation(RPWR.Quaternion());

	BlueDoor->PortalViewCapture->SetWorldTransform(ULinkooTools::CaculTransformForPortal(TransBlue, RedDoor->GetActorTransform(), BlueDoor->GetActorTransform()));
	RedDoor->PortalViewCapture->SetWorldTransform(ULinkooTools::CaculTransformForPortal(TransRed, BlueDoor->GetActorTransform(), RedDoor->GetActorTransform()));

	// 设置裁剪平面
	BlueDoor->PortalViewCapture->ClipPlaneBase = BlueDoor->GetActorLocation();
	RedDoor->PortalViewCapture->ClipPlaneBase = RedDoor->GetActorLocation();
	BlueDoor->PortalViewCapture->ClipPlaneNormal = BlueDoor->GetActorForwardVector();
	RedDoor->PortalViewCapture->ClipPlaneNormal = RedDoor->GetActorForwardVector();
}

bool APortalDoorManager::IsAllReady()
{
	if (BlueDoor.IsValid() && RedDoor.IsValid() && BlueDoor->IsActive() && RedDoor->IsActive()) return true;
	return false;
}
