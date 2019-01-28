// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineWall.generated.h"

UCLASS()
class PROJECTBALL_API ASplineWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void Refresh(const TArray<FVector>& InPoints);
	UFUNCTION()
	void Callback_FinishBuild();

	void PositionEdit(int32 TargetIdx, FVector FinalPos);
	void PositionMerge(int32 TargetIdx);

	void SetAllDisablePositionEdit();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASplineWall")
	class USplineComponent*		SplineComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UStaticMesh*			WallStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UMaterialInstanceConstant*	WallMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UMaterialInstanceConstant*	FinalWallMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UMaterialInstanceDynamic*		WallMaterialDynamic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UBlueprintGeneratedClass* ColomnClass;


private:
	float RadiusOffSet = 15.f;
	float TotalWallLength = 0.f;
	/*Activated*/
	bool bActivated = false;

	float SpawnEffectDuration = 0.5f;
	float SpawnEffectElapsedTime = 1.f;

	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshArray;
	UPROPERTY()
	TArray<class AWallColumn*> ColumnActorArray;
	UPROPERTY()
	TArray<FVector> CachedPointArray;

private:
	const FName APPEAR_PARAM = TEXT("control");

public:
	FORCEINLINE bool isActivated() const { return bActivated == true; }
	FORCEINLINE float GetTotalLength() const { return TotalWallLength; }

};
