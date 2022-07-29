
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LinkooTools.generated.h"

/**
 * 
 */
UCLASS()
class LINKOOPORTAL_API ULinkooTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	ULinkooTools();
	~ULinkooTools();
	// 计算A点关于以Ori为原点，Normal为法线的平面的镜面反射点A'
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector CaculReflectLocation(const FVector& DotA,const FVector& DotOri, const FVector& Normal);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	// 计算A向量以以Normal为法向量的镜面反射后的向量A'
	static FVector CaculReflectVector(const FVector& VecA, const FVector& Normal);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	// A点绕轴旋转180度后的点的坐标
	static FVector CaculReversOfAxis(const FVector& LocA,const FVector& Ori,const FVector& AxisVector);
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	// 用三者的世界变换计算Obj相对与A的关系转移到B的坐标系
	static inline FTransform CaculTransformForPortal(const FTransform& Obj,const FTransform& A,const FTransform& B )
	{
		return UKismetMathLibrary::ComposeTransforms(UKismetMathLibrary::MakeRelativeTransform(Obj, A), B);
	}

	UFUNCTION(BlueprintCallable,BlueprintPure)
	static inline bool AIsFrontOfB (AActor* A, AActor* B)
	{
		return FVector::DotProduct(A->GetActorLocation() - B->GetActorLocation(), B->GetActorForwardVector()) > 0.0f;
	}

	static inline bool AIsFrontOfB (USceneComponent* A, AActor* B)
	{
		return FVector::DotProduct(A->GetComponentLocation() - B->GetActorLocation(), B->GetActorForwardVector()) > 0.0f;
	}
	static inline float DistantOfA2Face(const FVector& LocA,const FVector& Ori,const FVector& AxisVector)
	{
		FVector ChuiZu = Ori + FVector::DotProduct(LocA - Ori, AxisVector.GetSafeNormal()) * AxisVector.GetSafeNormal() ;
		return (ChuiZu-LocA).Size();
	};

	// 返回Loc与A的相对向量转移到B的相对位置
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector CaculRelativeLocationFromA2B(FVector Loc, AActor* A, AActor* B);
	
};
