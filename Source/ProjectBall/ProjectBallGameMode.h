// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectBallGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameModeState : uint8
{
	IDLE,
	PLAY,
	END
};



UCLASS(minimalapi)
class AProjectBallGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectBallGameMode();

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StageFail();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StageSuccess();


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EGameModeState, eModestate);
	UPROPERTY(BlueprintAssignable)
	FOnStateChanged OnStateChanged;

	UFUNCTION(BlueprintCallable)
	EGameModeState GetCurrentMode();
	UFUNCTION(BlueprintCallable)
	void SetCurrentMode(EGameModeState NewState);

	

private:
	UPROPERTY()
	EGameModeState CurrentMode = EGameModeState::IDLE;
};



