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
	void Refresh(TArray<FVector> InPoints);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASplineWall")
	class USplineComponent*		SplineComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UStaticMesh*			WallStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UMaterialInterface*	WallMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UStaticMesh*			ColStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASplineWall")
	class UMaterialInterface*	ColMaterial;


private:
	float TotalWallLength = 0.f;
	/*Activated*/
	bool bActivated = false;

	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshArray;

public:
	FORCEINLINE bool isActivated() const { return bActivated == true; }
	FORCEINLINE float GetTotalLength() const { return TotalWallLength; }

};
