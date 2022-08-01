// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCube.h"

#include "CanBeGrab.h"
#include "LinkooTools.h"
#include "Kismet/GameplayStatics.h"
#include "LinkooPortal/LinkooPortal.h"
#include "LinkooPortal/LinkooPortalCharacter.h"

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
	NewActor->MainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MainMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_OnlyCanGrab, ECollisionResponse::ECR_Block);
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

void AGameCube::OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)
{
	AActor** ServantActorPtr = PortalHelper->MasterServantMap.Find(this);
	if (ServantActorPtr)
	{
		(*ServantActorPtr)->SetActorHiddenInGame(false);
	}
	else
	{
		AActor* ServantActor =  SpawnCopyActor();
	
		PortalHelper->AllCopyActors.Add(ServantActor);
		PortalHelper->MasterServantMap.Add(this, ServantActor);
	}

	// 将Actor加入Array和Set引用
	if (Cast<APortalDoor>(OverlappedComponent->GetOwner())->DoorType == EPortalDoorType::Blue)
	{
		PortalHelper->ActorsNearBlueDoor.Add(this);
	}
	else
	{
		PortalHelper->ActorsNearRedDoor.Add(this);
	}

}

void AGameCube::OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)
{
	if (ULinkooTools::AIsFrontOfB(this, OverlappedComponent->GetOwner()))
	{
		// 正面出去则是正常出
		PortalHelper->ActorsNearRedDoor.Remove(this);
		PortalHelper->ActorsNearBlueDoor.Remove(this);
		if(PortalHelper->MasterServantMap[this]) PortalHelper->MasterServantMap[this]->SetActorHiddenInGame(true);
	}
	else
	{
		// 从后面出去说明准备传送, 速度计算公式：RotatorB * Inv(RotatorA) * VelocityVector
		PortalHelper->SwitchMasterServant(this);
		const APortalDoor* OtherDoor = Cast<APortalDoor>(OverlappedComponent->GetOwner())->GetTheOtherPortal();
		if(FVector::DotProduct(OtherDoor->GetActorForwardVector(), FVector(0,0,1))> 0.9)
		{
			// 门朝天时给个最小速度5m/s
			this->FindComponentByClass<UPrimitiveComponent>()->SetPhysicsLinearVelocity(FMath::Clamp(this->GetVelocity().Size(), 300.0f ,4500.0f) * OtherDoor->GetActorForwardVector());
		}
		else
		{
			this->FindComponentByClass<UPrimitiveComponent>()->SetPhysicsLinearVelocity(FMath::Clamp(this->GetVelocity().Size(), 10.0f ,4500.0f) * OtherDoor->GetActorForwardVector());
		}
	}
}

void AGameCube::OnInnerOverlapBegin(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)
{
	MainMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
}

void AGameCube::OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper)
{
	MainMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);

}

void AGameCube::OnEnterPortalTick(APortalDoor* NearDoor, AActor* CopyActor)
{
	CopyActor->SetActorTransform(ULinkooTools::CaculTransformForPortal(FTransform(UKismetMathLibrary::MakeRotFromXZ(UKismetMathLibrary::RotateAngleAxis(GetActorForwardVector(), 180.0f ,NearDoor->GetActorUpVector()), UKismetMathLibrary::RotateAngleAxis(GetActorUpVector(),180.0f, NearDoor->GetActorUpVector())), ULinkooTools::CaculReversOfAxis(GetActorLocation(), NearDoor->GetActorLocation(), NearDoor->GetActorUpVector()), GetActorScale()),NearDoor->GetTransform(), NearDoor->GetTheOtherPortal()->GetTransform()));
}

void AGameCube::OnSwitchMasterServant(AActor* CopyActor, UPortalHelperComponent* PortalHelper)
{
	auto GameCharact = Cast<ALinkooPortalCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	PortalHelper->ActorsNearBlueDoor.Remove(this);
	PortalHelper->ActorsNearRedDoor.Remove(this);
	if (!GameCharact->IsActorEquelHandle(this))
	{
		SetActorTransform(CopyActor->GetTransform() );
	}
	else
	{
		GameCharact->ReversGrabMode();

	}
}
