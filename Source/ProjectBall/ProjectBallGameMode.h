// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectBallGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameModeState : uint8
{
	TUTORIAL,
	IDLE,
	PLAY,
	END
};

UENUM(BlueprintType)
enum class ETutorialMode : uint8
{
	TUTO1,
	TUTO2,
	TUTO3,
	TUTO4,
	TUTOEND
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialModeChanged, ETutorialMode, eTutoModeState);
	UPROPERTY(BlueprintAssignable)
	FOnTutorialModeChanged OnTutoModeChanged;

	UFUNCTION(BlueprintCallable)
	EGameModeState GetCurrentMode();
	UFUNCTION(BlueprintCallable)
	void SetCurrentMode(EGameModeState NewState);

	UFUNCTION(BlueprintCallable)
	ETutorialMode GetTutorialMode();
	UFUNCTION(BlueprintCallable)
	void SetCurrentTutorialMode(ETutorialMode NewMode);
	

private:
	UPROPERTY()
	EGameModeState CurrentMode = EGameModeState::TUTORIAL;
	UPROPERTY()
	ETutorialMode TutoMode = ETutorialMode::TUTO1;
};



