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

public:
	bool bTutorialCompleted = false;
	
};
