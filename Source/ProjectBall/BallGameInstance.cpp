// Fill out your copyright notice in the Description page of Project Settings.

#include "BallGameInstance.h"
#include "ProjectBall.h"

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
