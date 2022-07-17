// Copyright Epic Games, Inc. All Rights Reserved.

#include "LinkooPortalCharacter.h"
#include "LinkooPortalProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALinkooPortalCharacter

ALinkooPortalCharacter::ALinkooPortalCharacter()
{
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
	PlayerInputComponent->BindAction("PressE", IE_Pressed, this, &ALinkooPortalCharacter::CrabObject);

	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &ALinkooPortalCharacter::test);
	
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

void ALinkooPortalCharacter::CrabObject()
{
}

void ALinkooPortalCharacter::test()
{
	if(FPortalDoorManager::Get().BlueDoor->PortalViewCapture->TextureTarget==nullptr)
	UE_LOG(LogTemp, Warning,TEXT("NAME, %ll"), FPortalDoorManager::Get().BlueDoor->PortalViewCapture->TextureTarget);
}

void ALinkooPortalCharacter::Fire(EPortalDoorType dtype)
{

	if (!bIsGrabObj)
	{
		FVector StartLocation;
		FVector EndLocation;
		FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(GetFirstPersonCameraComponent()->GetComponentRotation());
		StartLocation = GetFirstPersonCameraComponent()->GetComponentLocation();
		// Ignore Array
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		auto PDM = FPortalDoorManager::Get();
		if (PDM.BlueDoor) IgnoreActors.Add(PDM.BlueDoor);
		if (PDM.RedDoor) IgnoreActors.Add(PDM.RedDoor);
		
		FHitResult HitResult;
		
		EndLocation = StartLocation + 100000*CameraForwardVector;

		bool HitSuccess = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false,
			IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);

		if (HitSuccess)
		{
			FTransform SpawnTransform;
			FRotator SpawnRotator = UKismetMathLibrary::MakeRotFromXY(HitResult.Normal, FVector::CrossProduct(FVector(0, 0, 1) ,StartLocation-HitResult.Location));
			SpawnTransform.SetLocation(HitResult.Location + 1 * HitResult.Normal);
			SpawnTransform.SetRotation(SpawnRotator.Quaternion());

			FPortalDoorManager::Get().SpawnOrActiveDoor(dtype, &SpawnTransform, this);
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
