// Fill out your copyright notice in the Description page of Project Settings.


#include "LinkooTools.h"

LinkooTools::LinkooTools()
{
}

LinkooTools::~LinkooTools()
{
}

FVector LinkooTools::CaculReflectLocation(const FVector& DotA, const FVector& DotOri, const FVector& Normal)
{
	return DotA -(2 * FVector::DotProduct(DotOri-DotA, Normal) * Normal);
}

FVector LinkooTools::CaculReflectVector(const FVector& VecA, const FVector& Normal)
{
	return VecA - 2 * FVector::DotProduct(VecA, Normal) * Normal;
}
