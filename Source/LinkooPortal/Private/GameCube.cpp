// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCube.h"

#include "CanBeGrab.h"

// Sets default values
AGameCube::AGameCube() 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssert(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (MeshAssert.Succeeded())
	{
		MainMesh->SetStaticMesh(MeshAssert.Object);
	}
	RootComponent = MainMesh;
	MainMesh->SetSimulatePhysics(true);

	MainMesh->SetWorldScale3D(FVector(0.6, 0.6, 0.6));
	
}

AActor* AGameCube::SpawnCopyActor()
{
	FActorSpawnParameters Parameters;
	Parameters.Template = this;
	FTransform Transform;
	Transform.SetLocation(FVector(0, 0, -1000.0f));
	Transform.SetScale3D(this->GetActorScale());
	// UE_LOG(LogTemp, Warning, TEXT(""))
	AGameCube* NewActor = Cast<AGameCube>(GetWorld()->SpawnActor(this->GetClass(), &Transform,  Parameters));
	NewActor->MainMesh->SetSimulatePhysics(false);
	NewActor->MainMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	

	return NewActor;
}

// Called when the game starts or when spawned
void AGameCube::BeginPlay()
{
	Super::BeginPlay();
	if (MeshMaterial) MainMesh->SetMaterial(0, MeshMaterial);
	
}

// Called every frame
void AGameCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

