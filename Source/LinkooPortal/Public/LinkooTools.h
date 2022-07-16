// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class LINKOOPORTAL_API LinkooTools
{
public:
	LinkooTools();
	~LinkooTools();

	// 计算A点关于以Ori为原点，Normal为法线的平面的镜面反射点A'
	static FVector CaculReflectLocation(const FVector& DotA,const FVector& DotOri, const FVector& Normal);

	// 计算A向量以以Normal为法向量的镜面反射后的向量A'
	static FVector CaculReflectVector(const FVector& VecA, const FVector& Normal);
};
