// Fill out your copyright notice in the Description page of Project Settings.

#include "StageHelper.h"

UStageHelper* UStageHelper::InstancePtr = nullptr;

UStageHelper* UStageHelper::Get()
{
	if (InstancePtr == nullptr)
	{
		InstancePtr = NewObject<UStageHelper>();
		InstancePtr->AddToRoot();
	}

	return InstancePtr;
}

void UStageHelper::Release()
{
	if (InstancePtr)
	{
		InstancePtr->RemoveFromRoot();
		InstancePtr = nullptr;
	}
}
