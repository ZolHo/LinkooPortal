// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalDoor.h"

#include "Kismet/GameplayStatics.h"
#include "LinkooPortal/LinkooPortalGameMode.h"



// Sets default values
APortalDoor::APortalDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/******                    初始化组件 START                             ******/
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScence"));
	
	DoorFrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrameMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssert(TEXT("StaticMesh'/Game/StarterContent/Props/SM_WindowFrame.SM_WindowFrame'"));
	if (MeshAssert.Succeeded())
	{
		DoorFrameMesh->SetStaticMesh(MeshAssert.Object);
	}
	DoorFrameMesh->SetRelativeScale3D(FVector(0.1, 2.0, 2.0));
	DoorFrameMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	DoorFaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFace"));
	DoorFaceMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssert2(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Plane.Shape_Plane'"));
	if (MeshAssert2.Succeeded())
	{
		DoorFaceMesh->SetStaticMesh(MeshAssert2.Object);
	}
	DoorFaceMesh->SetRelativeScale3D(FVector(2.5, 1.5, 1.0));
	DoorFaceMesh->SetRelativeRotation(FRotator(270.0, 0.0, 0.0));
	DoorFaceMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	DoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorCollision"));
	DoorCollision->SetupAttachment(DoorFaceMesh);
	DoorCollision->SetRelativeScale3D(FVector(1.5, 1.5, 0.1));
	
	/******                    初始化组件 END                             ******/
}

// Called when the game starts or when spawned
void APortalDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APortalDoor::InitDoor(EPortalDoorType dtype)
{
	
	if (dtype == EPortalDoorType::Blue)
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAssert(TEXT("Material'/Game/MaterialSource/MyMaterials/M_Frame_Blue.M_Frame_Blue'"));
		if(MaterialAssert.Succeeded()) DoorFrameMesh->SetMaterial(0, MaterialAssert.Object);
	} else
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAssert(TEXT("Material'/Game/MaterialSource/MyMaterials/M_Frame_Red.M_Frame_Red'"));
		if(MaterialAssert.Succeeded()) DoorFrameMesh->SetMaterial(0, MaterialAssert.Object);
	}

}

const APortalDoor* APortalDoor::GetTheOtherPortal()
{
	
	if(TheOtherDoor) return TheOtherDoor;
	
	// 从Manager获得另一个传送门
	auto PDM = FPortalDoorManager::Get();
	if (DoorType==EPortalDoorType::Blue) {TheOtherDoor = PDM.RedDoor;}
	else TheOtherDoor = PDM.BlueDoor;
	
	return TheOtherDoor;
}

// Called every frame
void APortalDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APortalDoor::GetDoorActive() const
{
	return ActiveState;
}

void APortalDoor::SetDoorActive(bool state)
{
	if (state == ActiveState) return;
	
	if (!state)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	} else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
	ActiveState = state;
}


/******                    定义Manager方法 START                             ******/
FPortalDoorManager& FPortalDoorManager::Get()
{
	{
		static FPortalDoorManager Singleton;
		return Singleton;
	}
}


void FPortalDoorManager::ResetManager()
{
	BlueDoor = nullptr;
	RedDoor = nullptr;
}

bool FPortalDoorManager::SpawnOrActiveDoor(EPortalDoorType dtype, FTransform* spawnTransform, AActor* const caller)
{
	// 利用指针的指针指向这次要操作的是红门还是蓝门
	APortalDoor** dealDoor;
	if (dtype == EPortalDoorType::Blue) dealDoor = &BlueDoor;
	else dealDoor = &RedDoor;

	if (!NowWorld || caller->GetWorld() != NowWorld) ResetManager();
	NowWorld = caller->GetWorld();
	
	if(IsValid(*dealDoor))
	{
		(*dealDoor)->SetDoorActive(true);
		(*dealDoor)->SetActorTransform(*spawnTransform);
	}
	else
	{
		// 创建新的传送门
		auto door = Cast<APortalDoor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(caller, APortalDoor::StaticClass(), *spawnTransform));
		door->InitDoor(dtype);
		(*dealDoor) = door;
		UGameplayStatics::FinishSpawningActor(door, *spawnTransform);
	}

	// TODO: 完善条件判断
	return true;
}

/******                    定义Manager方法 END                             ******/