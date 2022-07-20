// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalDoor.h"

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
	DoorFrameMesh->SetRelativeScale3D(FVector(0.01, 2.0, 2.0));
	DoorFrameMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	DoorFaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFace"));
	DoorFaceMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssert2(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Plane.Shape_Plane'"));
	if (MeshAssert2.Succeeded())
	{
		DoorFaceMesh->SetStaticMesh(MeshAssert2.Object);
	}
	DoorFaceMesh->SetRelativeScale3D(FVector(2.5, 1.5, 0.01));
	DoorFaceMesh->SetRelativeRotation(FRotator(270.0, 0.0, 0.0));
	DoorFaceMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	DoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorCollision"));
	DoorCollision->SetupAttachment(RootComponent);
	DoorCollision->SetRelativeScale3D(FVector(1, 2, 3.5));

	InnerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InnerCollision"));
	InnerCollision->SetupAttachment(RootComponent);
	InnerCollision->SetRelativeScale3D(FVector(0.1, 2, 3.5));

	PortalViewCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ViewCapture"));
	PortalViewCapture->SetupAttachment(RootComponent);
	PortalViewCapture->bEnableClipPlane = true;
	
	/******                    初始化组件 END                             ******/
	
}

// Called when the game starts or when spawned
void APortalDoor::BeginPlay()
{
	Super::BeginPlay();

}

const APortalDoor* APortalDoor::GetTheOtherPortal()
{
	return TheOtherDoor.Get();
}

// Called every frame
void APortalDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APortalDoor::IsActive() const
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
		DoorFaceMesh->SetMaterial(0, DoorFaceMaterial);
		// DoorCollision->SetActive(false);
		// InnerCollision->SetActive(false);
		
	} else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
	ActiveState = state;
}