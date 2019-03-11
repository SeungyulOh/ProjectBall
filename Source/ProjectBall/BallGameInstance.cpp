// Fill out your copyright notice in the Description page of Project Settings.

#include "BallGameInstance.h"
#include "ProjectBall.h"
#include "StageHelper.h"
#include "BallPlayerController.h"
#include "SplineWall.h"

PROJECTBALL_API UBallGameInstance* BallGameInstance = nullptr;

class UBallGameInstance* UBallGameInstance::Get(class UObject* OutterOwner)
{
#ifdef WITH_EDITOR
	if (OutterOwner == nullptr || OutterOwner->GetWorld() == nullptr)
	{
		return BallGameInstance;
	}

	UBallGameInstance* GameInstance = Cast<UBallGameInstance>(OutterOwner->GetWorld()->GetGameInstance());
	if (GameInstance == nullptr)
	{
		ensure(GameInstance == nullptr);
		return BallGameInstance;
	}
	return GameInstance;
#else
	return BallGameInstance;
#endif
}

void UBallGameInstance::Init()
{
	Super::Init();

	BallGameInstance = this;

	WallMemoryHelper = NewObject<UWallMemoryHelper>();
}

void UBallGameInstance::Shutdown()
{
	Super::Shutdown();

	UStageHelper::Get()->Release();
	WallMemoryHelper = nullptr;
}

void UWallMemoryHelper::Clear()
{
	WallInfos.Empty();
}

bool UWallMemoryHelper::isWallMemoryExist()
{
	return WallInfos.Num();
}

void UWallMemoryHelper::SavePointVectors(TArray<FPointVectors> InWallInfo)
{
	WallInfos = InWallInfo;
}

void UWallMemoryHelper::MakeWall(class ABallPlayerController* BallPC)
{
	for (FPointVectors& Element : WallInfos)
	{
		ASplineWall* Wall = BallPC->SpawnSplineWall();
		if (IsValid(Wall))
		{
			Wall->MakeWall(Element.Points);
		}
	}
}
