// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BallGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBALL_API UBallGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	static class UBallGameInstance* Get(class UObject* OutterOwner);
	
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	bool bTutorialCompleted = false;

	UPROPERTY()
	class UWallMemoryHelper* WallMemoryHelper;
	
};

USTRUCT()
struct FPointVectors
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FVector> Points;
};

UCLASS()
class PROJECTBALL_API UWallMemoryHelper : public UObject
{
	GENERATED_BODY()

public:
	void Clear();
	bool isWallMemoryExist();

	void SavePointVectors(TArray<FPointVectors> InWallInfo);

	void MakeWall(class ABallPlayerController* BallPC);

private:
	UPROPERTY()
	TArray<FPointVectors> WallInfos;

};