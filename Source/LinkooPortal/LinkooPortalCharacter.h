// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "CanEnterPortal.h"
#include "PortalDoor.h"
#include "PortalHelperComponent.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "LinkooPortalCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ALinkooPortalCharacter : public ACharacter, public ICanEnterPortal
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;
	// UStaticMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly)
	UPhysicsHandleComponent* MyHandleComponent;

public:
	ALinkooPortalCharacter();

	void ReversGrabMode();

	void SetGrabMode(bool Mode);

	// // 计算人面前的门
	// void RecastDoorBetweenPawnAndObject();
	
protected:
	virtual void BeginPlay();

	// 计算手持物品的位置模式，true为普通射线检测，false为传送门中转模式
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bGrabActorMode = true;

	// 保存人面前的那扇门用于抓取Actor
	TWeakObjectPtr<APortalDoor> DoorWhichBetweenHandleActor;
	
public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ALinkooPortalProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	// 传送门管理器
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APortalDoorManager* PDM;

public:

	// 是否抓着东西
	bool bIsGrabObj = false;

	// 松手，主动松手和被阻挡时自动松手
	UFUNCTION(BlueprintCallable)
	void ReleaseHandleActor();

	// 抓东西，考虑穿墙和不穿墙两种方式
	void TraceAndGrabActor();
	
	// 以下三个时为了解决物理抓柄和瞬移之间的冲突
	FTimerHandle SetHandleCollisionTimerHandle;
	void SetHandleNoCollisionUntilNextFrame();
	void ExecuteTimer();
	bool bPreGrabMode;

	// 是否需要传送
	bool bNeedSwap = false;
	TWeakObjectPtr<UPortalHelperComponent> PHC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APortalDoor* NowInDoor;
	void RealSwitch(AActor* CopyActor, UPortalHelperComponent* PortalHelper);
	// 比较是不是手上拿的Actor
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActorEquelHandle(AActor* TheActor);

	//检查是否倾斜
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ChekIsBodyBias();
	// // 恢复摄像机的旋转
	UFUNCTION(BlueprintCallable)
	void RecureCameraRot(float DeltaSeconds) ;

	// 清除传送门
	UFUNCTION(BlueprintCallable)
	void CleanDoor();
protected:

	// 左键蓝门
	void LeftFire();

	// 右键红门
	void RightFire();

	// 拾取
	void GrabObject();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// 真正的开传送枪
	void Fire(EPortalDoorType dtype);

	// void test();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// 更改传送门监控位置
	virtual void Tick(float DeltaSeconds) override;

public :
	virtual AActor* SpawnCopyActor() override;
	virtual void OnOuterOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		UPortalHelperComponent* PortalHelper) override;
	virtual void
	OnOuterOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper) override;
	virtual void OnInnerOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		UPortalHelperComponent* PortalHelper) override;
	virtual void
	OnInnerOverlapEnd(UPrimitiveComponent* OverlappedComponent, UPortalHelperComponent* PortalHelper) override;

	virtual void OnEnterPortalTick(APortalDoor* NearDoor, AActor* CopyActor) override;
	virtual void OnSwitchMasterServant(AActor* CopyActor, UPortalHelperComponent* PortalHelper) override;
};

