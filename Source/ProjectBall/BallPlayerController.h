// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BallPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBALL_API ABallPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABallPlayerController();
	virtual void SetupInputComponent() override;
	
public:
	DECLARE_DELEGATE_OneParam(FOnSelectedPoint, TArray<FVector>);
	FOnSelectedPoint OnSelectedPoint;

public:
	UFUNCTION()
	void CallbackInputTouchBegin(ETouchIndex::Type TouchIndex, FVector Location);
	UFUNCTION()
	void CallbackInputTouchOver(ETouchIndex::Type TouchIndex, FVector Location);
	UFUNCTION()
	void CallbackInputTouchEnd(ETouchIndex::Type TouchIndex, FVector Location);

private:
	/*Allow just 1 touch for now*/
	ETouchIndex::Type CurrentTouchType = ETouchIndex::MAX_TOUCHES;
	FVector2D StartPos;
	FVector2D CurrentPos;
	FVector2D DirectionVector;

	UPROPERTY()
	TArray<FVector> PointArray;
};
