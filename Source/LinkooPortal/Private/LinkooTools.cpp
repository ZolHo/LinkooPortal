// Fill out your copyright notice in the Description page of Project Settings.


#include "LinkooTools.h"

ULinkooTools::ULinkooTools()
{
}

ULinkooTools::~ULinkooTools()
{
}
FVector ULinkooTools::CaculReflectLocation(const FVector& DotA, const FVector& DotOri, const FVector& Normal)
{
	return DotA -(2 * FVector::DotProduct(DotA - DotOri, Normal) * Normal);
}

FVector ULinkooTools::CaculReflectVector(const FVector& VecA, const FVector& Normal)
{
	return VecA - 2 * FVector::DotProduct(VecA, Normal) * Normal;
}
