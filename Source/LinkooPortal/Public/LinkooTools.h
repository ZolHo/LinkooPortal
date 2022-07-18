
#pragma once

#include "CoreMinimal.h"
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
	UFUNCTION(BlueprintCallable)
	static FVector CaculReflectLocation(const FVector& DotA,const FVector& DotOri, const FVector& Normal);

	UFUNCTION(BlueprintCallable)
	// 计算A向量以以Normal为法向量的镜面反射后的向量A'
	static FVector CaculReflectVector(const FVector& VecA, const FVector& Normal);
};
