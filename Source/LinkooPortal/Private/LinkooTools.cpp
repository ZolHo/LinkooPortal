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

FVector ULinkooTools::CaculReversOfAxis(const FVector& LocA,const FVector& Ori,const FVector& AxisVector)
{
	FVector ChuiZu = Ori + FVector::DotProduct(LocA - Ori, AxisVector.GetSafeNormal()) * AxisVector.GetSafeNormal()  ;
	return LocA + 2 * ( ChuiZu - LocA);
}

FVector ULinkooTools::CaculRelativeLocationFromA2B(FVector Loc, AActor* A, AActor* B)
{
	FVector RelativVector = Loc - A->GetActorLocation();
	return B->GetActorRotation().RotateVector(A->GetActorRotation().UnrotateVector(RelativVector));
	
}
