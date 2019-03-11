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

	friend class UWallMemoryHelper;

public:
	ABallPlayerController();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	DECLARE_DELEGATE_OneParam(FOnSelectedPoint, const TArray<FVector>&);
	FOnSelectedPoint OnSelectedPoint;
	DECLARE_DELEGATE(FOnFinishedWall)
	FOnFinishedWall OnFinishedWall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Point")
	bool isThumbOnGoalPoint = false;

public:
	UFUNCTION()
	void CallbackInputTouchBegin(ETouchIndex::Type TouchIndex, FVector Location);
	UFUNCTION()
	void CallbackInputTouchOver(ETouchIndex::Type TouchIndex, FVector Location);
	UFUNCTION()
	void CallbackInputTouchEnd(ETouchIndex::Type TouchIndex, FVector Location);
	

	UFUNCTION()
	void CallbackInputTouch2();

	UFUNCTION(BlueprintCallable)
	void PlayStart();

	UFUNCTION(BlueprintCallable)
	void SaveWallPoints();

	
	void DestroyAllSplineWall();
	void RemoveElementInVector(class ASplineWall* InWall);

private:
	class ASplineWall* SpawnSplineWall();

private:
	/*Allow just 1 touch for now*/
	ETouchIndex::Type CurrentTouchType = ETouchIndex::MAX_TOUCHES;

	UPROPERTY(EditDefaultsOnly)
	UBlueprintGeneratedClass* SplineWallClass;

	UPROPERTY()
	TArray<FVector> PointArray;

	UPROPERTY()
	UMaterialInstanceDynamic* MID;

	int32 PointNum;
	
	UPROPERTY()
	TWeakObjectPtr<class AWallColumn> PositionEditingWallColumn;

	UPROPERTY()
	TWeakObjectPtr<class ATutorialPoint> LastTouchedTutoPoint;

	UPROPERTY()
	TArray<class ASplineWall*> SplineWallArray;

	UPROPERTY()
	TArray<class ASplineWall*> MergeWaitingList;


	/*Rendering Effect*/
	float DepthEffectDuration = 0.5f;
	float DepthEffectElapsedTime = 0.5f;
	bool bEnable = false;
};
