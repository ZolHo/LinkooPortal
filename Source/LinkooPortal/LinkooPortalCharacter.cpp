// Copyright Epic Games, Inc. All Rights Reserved.

#include "LinkooPortalCharacter.h"

#include <functional>

#include "CanBeGrab.h"
#include "LinkooPortalProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "PortalDoorManager.h"
#include "LinkooPortal.h"
#include "LinkooTools.h"
#include "PortalWall.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALinkooPortalCharacter

ALinkooPortalCharacter::ALinkooPortalCharacter()
{
	PrimaryActorTick.TickGroup = ETickingGroup::TG_EndPhysics;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	// FP_Gun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	MyHandleComponent = CreateDefaultSubobject<UPhysicsHandleComponent> (TEXT("PhyicsHandle"));
	MyHandleComponent->bRotationConstrained = true;
	// MyHandleComponent->SetInterpolationSpeed(1000000.0f);
	
}

void ALinkooPortalCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALinkooPortalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &ALinkooPortalCharacter::LeftFire);
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &ALinkooPortalCharacter::RightFire);

	// 绑定拾取按键
	PlayerInputComponent->BindAction("PressE", IE_Pressed, this, &ALinkooPortalCharacter::GrabObject);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ALinkooPortalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALinkooPortalCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALinkooPortalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALinkooPortalCharacter::LookUpAtRate);
}

void ALinkooPortalCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALinkooPortalCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALinkooPortalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALinkooPortalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ALinkooPortalCharacter::LeftFire()
{
	Fire(EPortalDoorType::Blue);
}

void ALinkooPortalCharacter::RightFire()
{
	Fire(EPortalDoorType::Red);
}

void ALinkooPortalCharacter::GrabObject()
{
	if (bIsGrabObj)
	{
		ReleaseHandleActor();
	}
	else
	{
		TraceAndGrabActor();
	}
}

void ALinkooPortalCharacter::ReleaseHandleActor()
{
	MyHandleComponent->GetGrabbedComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	MyHandleComponent->ReleaseComponent();

	bIsGrabObj = false;

	SetGrabMode(true);
	// MyHandleComponent->bInterpolateTarget = true;
}

void ALinkooPortalCharacter::TraceAndGrabActor()
{
	FVector StartLocation;
    FVector EndLocation;
    FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(GetFirstPersonCameraComponent()->GetComponentRotation());
    StartLocation = GetFirstPersonCameraComponent()->GetComponentLocation();

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);
    
    FHitResult HitResult;
    
    EndLocation = StartLocation + 300 * CameraForwardVector;

    bool HitSuccess = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Camera), false,
    	IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);

    if (HitSuccess)
    {
    	// UE_LOG(LogTemp, Error, TEXT("%s"), *HitResult.GetActor()->GetFullName());
    	if (Cast<ICanBeGrab> (HitResult.GetActor()))
    	{
    		SetGrabMode(true);
    		// MyHandleComponent->GrabComponentAtLocation(HitResult.GetComponent(), FName(""), HitResult.GetComponent()->GetCenterOfMass());
    		FVector MineXVector = GetFirstPersonCameraComponent()->GetRightVector()*-1.0f;
    		MineXVector.Z = 0.f;
            MyHandleComponent->GrabComponentAtLocationWithRotation(HitResult.GetComponent(), FName(""), HitResult.GetComponent()->GetCenterOfMass(), UKismetMathLibrary::MakeRotFromXZ(MineXVector, FVector(0,0,1)));
            HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

            bIsGrabObj = true;
    		MyHandleComponent->bInterpolateTarget = false;
    	}
    	else if( Cast<APortalDoor>(HitResult.GetActor()))
    	{
    		// 如果人物想穿墙拿东西，则需要从对面门的摄像机发出穿透门的光线
    		// DoorWhichBetweenHandleActor = Cast<APortalDoor>(HitResult.GetActor());
    		StartLocation = NowInDoor->GetTheOtherPortal()->PortalViewCapture->GetComponentLocation();
    		EndLocation = StartLocation + 300 * NowInDoor->GetTheOtherPortal()->PortalViewCapture->GetForwardVector();
    		FHitResult PortalHitResult;
    		HitSuccess = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_OnlyCanGrab), false,
		IgnoreActors, EDrawDebugTrace::ForDuration, PortalHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);
    		if (HitSuccess)
    		{
    			// 更改handle 模式为穿墙持有
    			SetGrabMode(false);
    			FVector MineXVector = NowInDoor->GetTheOtherPortal()->PortalViewCapture->GetRightVector() * -1.f;
    			MineXVector.Z = 0.f;
    			MyHandleComponent->GrabComponentAtLocationWithRotation(PortalHitResult.GetComponent(), FName(""), PortalHitResult.GetComponent()->GetCenterOfMass(),UKismetMathLibrary::MakeRotFromXZ(MineXVector, FVector(0,0,1)));
    			PortalHitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    		
    			bIsGrabObj = true;
    			MyHandleComponent->bInterpolateTarget = false;
    		}
    		
    	}
    }
}

void ALinkooPortalCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	if (MyHandleComponent->GetGrabbedComponent())
	{
		if (bPreGrabMode != bGrabActorMode)
		{
			SetHandleNoCollisionUntilNextFrame();
			bPreGrabMode = bGrabActorMode;

		}
		
		if (bGrabActorMode)
		{
			FVector MineXVector = GetFirstPersonCameraComponent()->GetRightVector() * -1.0f;
			MineXVector.Z = 0.f;
			auto TempRotation = UKismetMathLibrary::MakeRotFromXZ(MineXVector, FVector(0,0,1));

			MyHandleComponent->SetTargetLocationAndRotation(GetFirstPersonCameraComponent()->GetComponentLocation() + GetFirstPersonCameraComponent()->GetForwardVector() * 150.0, TempRotation);
			// MyHandleComponent->GetGrabbedComponent()->SetWorldLocation(GetFirstPersonCameraComponent()->GetComponentLocation() + GetFirstPersonCameraComponent()->GetForwardVector() * 150.0);
			// auto TempTransofm = FTransform(TempRotation.Quaternion(), GetFirstPersonCameraComponent()->GetComponentLocation() + GetFirstPersonCameraComponent()->GetForwardVector() * 150.0, FVector(1,1,1));
			// MyHandleComponent->GetGrabbedComponent()->GetBodyInstance()->SetBodyTransform(TempTransofm, ETeleportType::ResetPhysics);
			// MyHandleComponent->CurrentTransform = TempTransofm;
			// MyHandleComponent->TargetTransform = TempTransofm;
		}
		else
		{
			
			auto View = NowInDoor->GetTheOtherPortal()->PortalViewCapture;
			FVector MineXVector = View->GetRightVector() * -1.f;
			MineXVector.Z = 0.f;
			FTransform TempTransform;
			auto TempRotation = UKismetMathLibrary::MakeRotFromXZ(MineXVector, FVector(0,0,1));
			TempTransform.SetRotation(TempRotation.Quaternion());
			TempTransform.SetLocation(View->GetComponentLocation() + View->GetForwardVector() * 150.0);
			// MyHandleComponent->SetTargetLocation(View->GetComponentLocation() + View->GetForwardVector() * 150.0);
			// MyHandleComponent->GetGrabbedComponent()->GetBodyInstance()->SetBodyTransform(TempTransform, ETeleportType::ResetPhysics);
			MyHandleComponent->SetTargetLocationAndRotation(View->GetComponentLocation() + View->GetForwardVector() * 150.0, TempRotation );
			
		}
	}
}

void ALinkooPortalCharacter::ReversGrabMode()
{
	bGrabActorMode = !bGrabActorMode;
}

void ALinkooPortalCharacter::SetGrabMode(bool Mode)
{
	bGrabActorMode = Mode;
	bPreGrabMode = Mode;
}

void ALinkooPortalCharacter::Fire(EPortalDoorType dtype)
{

	if (!bIsGrabObj && PDM)
	{
		FVector StartLocation;
		FVector EndLocation;
		FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(GetFirstPersonCameraComponent()->GetComponentRotation());
		StartLocation = GetFirstPersonCameraComponent()->GetComponentLocation();
		// Ignore Array
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		if (dtype==Blue)
			IgnoreActors.Add(PDM->BlueDoor.Get());
		else 
			IgnoreActors.Add(PDM->RedDoor.Get());
		
		FHitResult HitResult;
		
		EndLocation = StartLocation + 100000*CameraForwardVector;

		bool HitSuccess = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
			IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Red, FLinearColor::Green, 3.f);

		if (HitSuccess  && Cast<APortalWall>(HitResult.Actor))
		{
			FTransform SpawnTransform;
			FRotator SpawnRotator = UKismetMathLibrary::MakeRotFromXY(HitResult.Normal, FVector::CrossProduct(FVector(0, 0, 1) ,StartLocation-HitResult.Location));
			SpawnTransform.SetLocation(HitResult.Location + 1 * HitResult.Normal);
			SpawnTransform.SetRotation(SpawnRotator.Quaternion());

			APortalDoor* Door = PDM->SpawnOrActiveDoor(dtype, SpawnTransform);
			if (Door)
			{
				Door->ActorWhichDoorStick = HitResult.GetActor();
			}
			
		}
	
		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	} else
	{
		// TODO: 持有东西时开枪应当发出操作失败的效果
	}
}

void ALinkooPortalCharacter::SetHandleNoCollisionUntilNextFrame()
{
	if (MyHandleComponent->GetGrabbedComponent())
	{
		// MyHandleComponent->GetGrabbedComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		MyHandleComponent->GetGrabbedComponent()->GetOwner()->SetActorEnableCollision(false);
		GetWorld()->GetTimerManager().SetTimer(SetHandleCollisionTimerHandle, this, &ALinkooPortalCharacter::ExecuteTimer, 0.001f, true);
	}
}

void ALinkooPortalCharacter::ExecuteTimer()
{
	if (MyHandleComponent->GetGrabbedComponent())
	{
		// MyHandleComponent->GetGrabbedComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MyHandleComponent->GetGrabbedComponent()->GetOwner()->SetActorEnableCollision(true);
		GetWorld()->GetTimerManager().ClearTimer(SetHandleCollisionTimerHandle);
	}
}

bool ALinkooPortalCharacter::IsActorEquelHandle(AActor* TheActor)
{
	if (bIsGrabObj)
	{
		return MyHandleComponent->GetGrabbedComponent()->GetOwner()==TheActor;
	}
	return false;
	
}

bool ALinkooPortalCharacter::ChekIsBodyBias()
{
	FRotator CameraRotator = GetController()->GetControlRotation();
	return UKismetMathLibrary::Abs(CameraRotator.Roll ) > 0.0001f;
}


void ALinkooPortalCharacter::RecureCameraRot(float DeltaSeconds)
{
	FVector ForwardVec = GetActorForwardVector();
	ForwardVec.Z = 0.0f;
	auto Temp = UKismetMathLibrary::MakeRotFromXZ(ForwardVec, FVector(0,0,1));
	SetActorRotation(Temp);
	GetController()->SetControlRotation(Temp);
}

void ALinkooPortalCharacter::CleanDoor()
{
	PDM->CleanDoors();
}

/**    ------------------------ 重载ICanEntryPortal --------------------------       **/


AActor* ALinkooPortalCharacter::SpawnCopyActor()
{
	FActorSpawnParameters Parameters;
	FTransform Transform;
	Transform.SetLocation(FVector(0, 0, -1000.0f));
	ALinkooPortalCharacter* NewActor = Cast<ALinkooPortalCharacter>(GetWorld()->SpawnActor(this->GetClass(), &Transform,  Parameters));
	NewActor->GetCapsuleComponent()->SetSimulatePhysics(false);
	NewActor->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewActor->SetActorTickEnabled(false);
	return NewActor;
}

void ALinkooPortalCharacter::OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	UPortalHelperComponent* PortalHelper)
{
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
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

	// NowInDoor = Cast<APortalDoor>(OverlappedComponent->GetOwner());
	// DoorWhichBetweenHandleActor = NowInDoor;
}

void ALinkooPortalCharacter::OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	UPortalHelperComponent* PortalHelper)
{
	// 正面出去则是正常出
	PortalHelper->ActorsNearRedDoor.Remove(this);
	PortalHelper->ActorsNearBlueDoor.Remove(this);
	if(PortalHelper->MasterServantMap[this]) PortalHelper->MasterServantMap[this]->SetActorHiddenInGame(true);
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
}

void ALinkooPortalCharacter::OnInnerOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	UPortalHelperComponent* PortalHelper)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	if (ULinkooTools::AIsFrontOfB(this->GetFirstPersonCameraComponent(), OverlappedComponent->GetOwner()))
	{ 
		PortalHelper->SwitchMasterServant(this);
	}

	// NowInDoor = Cast<APortalDoor>(OverlappedComponent->GetOwner());
	
}

void ALinkooPortalCharacter::OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	UPortalHelperComponent* PortalHelper)
{
	
	bNeedSwap = false;
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
}

void ALinkooPortalCharacter::OnEnterPortalTick(APortalDoor* NearDoor, AActor* CopyActor)
{
	
	CopyActor->SetActorTransform(ULinkooTools::CaculTransformForPortal(FTransform(UKismetMathLibrary::MakeRotFromXZ(UKismetMathLibrary::RotateAngleAxis(GetActorForwardVector(), 180.0f ,NearDoor->GetActorUpVector()), UKismetMathLibrary::RotateAngleAxis(GetActorUpVector(),180.0f, NearDoor->GetActorUpVector())), ULinkooTools::CaculReversOfAxis(GetActorLocation(), NearDoor->GetActorLocation(), NearDoor->GetActorUpVector()), GetActorScale()),NearDoor->GetTransform(), NearDoor->GetTheOtherPortal()->GetTransform()));

	Cast<ALinkooPortalCharacter>(CopyActor)->GetFirstPersonCameraComponent()->SetWorldTransform(ULinkooTools::CaculTransformForPortal(FTransform(UKismetMathLibrary::MakeRotFromXZ(UKismetMathLibrary::RotateAngleAxis(GetFirstPersonCameraComponent()->GetForwardVector(), 180.0f ,NearDoor->GetActorUpVector()), UKismetMathLibrary::RotateAngleAxis(GetFirstPersonCameraComponent()->GetUpVector(),180.0f, NearDoor->GetActorUpVector())), ULinkooTools::CaculReversOfAxis(GetFirstPersonCameraComponent()->GetComponentLocation(), NearDoor->GetActorLocation(), NearDoor->GetActorUpVector())),NearDoor->GetTransform(), NearDoor->GetTheOtherPortal()->GetTransform()));
	if (bNeedSwap)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("-- Test %s"), *NowInDoor->GetName()));
		// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("-- Test %f"), ULinkooTools::DistantOfA2Face(GetFirstPersonCameraComponent()->GetComponentLocation(), NowInDoor->GetActorLocation(), NowInDoor->GetActorUpVector())));
		if (ULinkooTools::DistantOfA2Face(GetFirstPersonCameraComponent()->GetComponentLocation(), NowInDoor->GetActorLocation(), NowInDoor->GetActorUpVector()) < 60.0f || !ULinkooTools::AIsFrontOfB(GetFirstPersonCameraComponent(), NowInDoor))
		{
			bNeedSwap = false;
			RealSwitch(PHC->MasterServantMap[this], PHC.Get());
		}
	}
}

void ALinkooPortalCharacter::OnSwitchMasterServant(AActor* CopyActor, UPortalHelperComponent* PortalHelper)
{
	
	bNeedSwap = true;
	PHC = PortalHelper;
	// SetActorLocation(CopyActor->GetActorLocation());
	// GetController()->SetControlRotation(Cast<ALinkooPortalCharacter>(CopyActor)->GetFirstPersonCameraComponent()->GetComponentRotation());
}

void ALinkooPortalCharacter::RealSwitch(AActor* CopyActor, UPortalHelperComponent* PortalHelper)
{
	PortalHelper->ActorsNearBlueDoor.Remove(this);
	PortalHelper->ActorsNearRedDoor.Remove(this);
	
	float Vel = this->GetVelocity().Size();

	GetController()->SetControlRotation(Cast<ALinkooPortalCharacter>(CopyActor)->GetFirstPersonCameraComponent()->GetComponentRotation());
	SetActorLocation(CopyActor->GetActorLocation() );
	bNeedSwap = false;
	OnEnterPortalTick(NowInDoor, CopyActor);

	if (bIsGrabObj)
	{
		ReversGrabMode();
	}
	
	if(FVector::DotProduct(NowInDoor->GetActorForwardVector(), FVector(0,0,1))> 0.9)
	{
		LaunchCharacter(FMath::Clamp(Vel, 800.0f ,8000.0f) * NowInDoor->GetActorForwardVector() , true, true);
	}
	else
	{
		LaunchCharacter(FMath::Clamp(Vel, 0.0f ,8000.0f) * NowInDoor->GetActorForwardVector() *1.f, true, true);
	}
	
}

/**    ------------------------ 重载ICanEntryPortal END --------------------------       **/
