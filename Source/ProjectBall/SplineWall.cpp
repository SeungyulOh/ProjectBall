// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineWall.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BallPlayerController.h"

// Sets default values
ASplineWall::ASplineWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASplineWall::BeginPlay()
{
	Super::BeginPlay();

	ABallPlayerController* BallPC = Cast<ABallPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (IsValid(BallPC))
	{
		BallPC->OnSelectedPoint.BindUObject(this, &ASplineWall::Refresh);
	}
	
	
}

// Called every frame
void ASplineWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASplineWall::Refresh(TArray<FVector> InPoints)
{
	if (IsValid(SplineComponent))
	{
		for (size_t i = 0; i < SplineMeshArray.Num(); ++i)
		{
			SplineMeshArray[i]->DestroyComponent();
		}
		SplineMeshArray.Empty();
		SplineComponent->ClearSplinePoints();
		TotalWallLength = 0.f;

		for (size_t i = 0; i < InPoints.Num(); ++i)
		{
			SplineComponent->AddSplinePoint(InPoints[i], ESplineCoordinateSpace::World);

			if (i == 0)
			{
				SetActorLocation(InPoints[i]);
			}
		}

		int32 TotalPoints = SplineComponent->GetNumberOfSplinePoints();
		int32 MeshCount = TotalPoints - 1;

		for (size_t i = 0; i < MeshCount; ++i)
		{
			USplineMeshComponent* meshComponent = NewObject<USplineMeshComponent>(this);
			if (meshComponent)
			{
				meshComponent->OnComponentCreated();
				meshComponent->SetForwardAxis(ESplineMeshAxis::X);
				meshComponent->SetStaticMesh(WallStaticMesh);
				meshComponent->SetRelativeScale3D(FVector(1.f, 0.5f, 1.f));
				//meshComponent->SetLocalRotatio(FRotator(0.f, 0.f, 90.f));
				int32 num = meshComponent->GetNumMaterials();
				meshComponent->SetMaterial(0,WallMaterial);

				FVector StartPos = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);

				FVector EndPos = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
				FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

				meshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
				meshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

				FVector Direction = StartPos - EndPos;
				TotalWallLength += Direction.Size2D();

				meshComponent->RegisterComponent();
				SplineMeshArray.Emplace(meshComponent);
			}
		}
	}
}

