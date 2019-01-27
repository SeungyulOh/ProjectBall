// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallColumn.generated.h"

UCLASS()
class PROJECTBALL_API AWallColumn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallColumn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void PositionEditable(bool bNewPositionEditable);

	void SetbPositionEditable(bool bNewPositionEditable);
	void EditPosition(FVector NewPos);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWallColumn")
	bool bPositionEditable = false;

	UPROPERTY()
	TWeakObjectPtr<class ASplineWall> ParentWall;

	int32 TargetIdx;

};
