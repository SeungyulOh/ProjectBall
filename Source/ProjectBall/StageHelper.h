// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StageHelper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBALL_API UStageHelper : public UObject
{
	GENERATED_BODY()

public:
	FORCEINLINE void AddCurrentStage() { ++CurrentStage; }
	FORCEINLINE int32 GetCurrentStage() { return CurrentStage; }
	FORCEINLINE void SetCurrentStage(int32 NewStage) {
		CurrentStage = NewStage;
	}

private:
	int32 CurrentStage = 1;

public:
	static UStageHelper* Get();
	void Release();

private:
	static UStageHelper* InstancePtr;

	UStageHelper() {};
	
};
