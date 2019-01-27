// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineWall.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BallPlayerController.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMathLibrary.h"
#include "WallColumn.h"
#include "DrawDebugHelpers.h"

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
		BallPC->OnFinishedWall.BindUObject(this, &ASplineWall::Callback_FinishBuild);
	}
	
	if(!IsValid(WallMaterialDynamic))
		WallMaterialDynamic = UMaterialInstanceDynamic::Create(FinalWallMaterial, this);

}

// Called every frame
void ASplineWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpawnEffectElapsedTime < SpawnEffectDuration)
	{
		SpawnEffectElapsedTime += DeltaTime;

		float Value = (SpawnEffectDuration - SpawnEffectElapsedTime) / SpawnEffectDuration;

		if (IsValid(WallMaterialDynamic))
			WallMaterialDynamic->SetScalarParameterValue(APPEAR_PARAM, Value);
	}

}

void ASplineWall::Refresh(const TArray<FVector>& InPoints)
{
	if (InPoints.Num() <= 1)
		return;

	

	//Update only for last point
	if (InPoints.Num() == CachedPointArray.Num())
	{
		int32 TargetIdx = SplineComponent->GetNumberOfSplinePoints() - 1;

		SplineComponent->RemoveSplinePoint(TargetIdx);
		if (InPoints.IsValidIndex(TargetIdx))
			SplineComponent->AddSplinePoint(InPoints[TargetIdx], ESplineCoordinateSpace::World);

		FVector StartPos = SplineComponent->GetLocationAtSplinePoint(TargetIdx -1, ESplineCoordinateSpace::World);
		FVector EndPos = SplineComponent->GetLocationAtSplinePoint(TargetIdx, ESplineCoordinateSpace::World);

		FVector Direction = EndPos - StartPos;
		float VectorLength = Direction.Size() - 2 * RadiusOffSet;
		Direction = Direction.GetSafeNormal();

		FRotator LookatRot = UKismetMathLibrary::FindLookAtRotation(StartPos, EndPos);
		//LookatRot.Yaw = 0.f;

		FTransform NewTransform;
		NewTransform.SetLocation(StartPos + Direction * RadiusOffSet + FVector(0.f, 100.f, 0.f));
		NewTransform.SetScale3D(FVector(VectorLength*0.01f, 2.f, 0.2f));
		NewTransform.SetRotation(LookatRot.Quaternion());

		if (SplineMeshArray.IsValidIndex(TargetIdx - 1))
		{
			SplineMeshArray[TargetIdx - 1]->DestroyComponent();
			SplineMeshArray.RemoveAt(TargetIdx - 1);

			USplineMeshComponent* meshComponent = NewObject<USplineMeshComponent>(this);
			if (meshComponent)
			{
				meshComponent->OnComponentCreated();

				meshComponent->SetStaticMesh(WallStaticMesh);
				meshComponent->SetMaterial(0, WallMaterial);
				meshComponent->SetWorldTransform(NewTransform);

				meshComponent->SetForwardAxis(ESplineMeshAxis::Z);
				meshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

				meshComponent->RegisterComponent();
				SplineMeshArray.Emplace(meshComponent);
			}
		}
			
		CachedPointArray = InPoints;
	}
	else
	{
		
		if (IsValid(SplineComponent))
		{
			TotalWallLength = 0.f;

			int32 LastIdx = InPoints.Num() - 1;
			SplineComponent->SetSplinePoints(InPoints, ESplineCoordinateSpace::World);

			int32 TotalPoints = SplineComponent->GetNumberOfSplinePoints();
			int32 MeshCount = TotalPoints - 1;

			USplineMeshComponent* meshComponent = NewObject<USplineMeshComponent>(this);
			if (meshComponent)
			{
				meshComponent->OnComponentCreated();

				meshComponent->SetStaticMesh(WallStaticMesh);
				meshComponent->SetMaterial(0, WallMaterial);

				FVector StartPos = SplineComponent->GetLocationAtSplinePoint(MeshCount-1, ESplineCoordinateSpace::World);
				FVector EndPos = SplineComponent->GetLocationAtSplinePoint(MeshCount, ESplineCoordinateSpace::World);

				FVector Direction = EndPos - StartPos;
				float VectorLength = Direction.Size() - 2 * RadiusOffSet;
				Direction = Direction.GetSafeNormal();

				FRotator LookatRot = UKismetMathLibrary::FindLookAtRotation(StartPos, EndPos);
				//LookatRot.Yaw = 0.f;

				FTransform NewTransform;
				NewTransform.SetLocation(StartPos + Direction * RadiusOffSet + FVector(0.f,100.f,0.f));
				NewTransform.SetScale3D(FVector(VectorLength*0.01f, 2.f, 0.2f));
				NewTransform.SetRotation(LookatRot.Quaternion());
				meshComponent->SetWorldTransform(NewTransform);
				
				meshComponent->SetForwardAxis(ESplineMeshAxis::Z);

				meshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				meshComponent->SetRenderCustomDepth(true);


				TotalWallLength += VectorLength;

				meshComponent->RegisterComponent();

				SplineMeshArray.Emplace(meshComponent);

				AWallColumn* Column = GetWorld()->SpawnActor<AWallColumn>(ColomnClass, StartPos, FRotator::ZeroRotator);
				if (IsValid(Column))
				{
					Column->TargetIdx = ColumnActorArray.Num();
					Column->ParentWall = this;
					ColumnActorArray.AddUnique(Column);
				}
			}

			CachedPointArray = InPoints;
		}
	}
}

void ASplineWall::Callback_FinishBuild()
{
	if (!CachedPointArray.Num())
		return;
	ABallPlayerController* BallPC = Cast<ABallPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (IsValid(BallPC))
	{
		BallPC->OnSelectedPoint.Unbind();
		BallPC->OnFinishedWall.Unbind();
	}

	int32 MaxIdx = CachedPointArray.Num() - 1;

	AWallColumn* Column = GetWorld()->SpawnActor<AWallColumn>(ColomnClass, CachedPointArray[MaxIdx], FRotator::ZeroRotator);
	if (IsValid(Column))
	{
		Column->TargetIdx = ColumnActorArray.Num();
		Column->ParentWall = this;
		ColumnActorArray.AddUnique(Column);

		for (USplineMeshComponent* Element : SplineMeshArray)
		{
			Element->SetMaterial(0, WallMaterialDynamic);
		}
		SpawnEffectElapsedTime = 0.f;

		for (AWallColumn* Element : ColumnActorArray)
			Element->SetbPositionEditable(true);
	}
}

void ASplineWall::PositionEdit(int32 TargetIdx, FVector FinalPos)
{
	if (CachedPointArray.IsValidIndex(TargetIdx))
		CachedPointArray[TargetIdx] = FinalPos;

	SplineComponent->SetSplinePoints(CachedPointArray, ESplineCoordinateSpace::World);

	if (CachedPointArray.IsValidIndex(TargetIdx - 1)) // left
	{
		FVector StartPos = SplineComponent->GetLocationAtSplinePoint(TargetIdx - 1, ESplineCoordinateSpace::World);
		FVector EndPos = SplineComponent->GetLocationAtSplinePoint(TargetIdx, ESplineCoordinateSpace::World);

		//DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Red, false, 1.f, 0, 3.f);

		FVector Direction = EndPos - StartPos;
		float VectorLength = Direction.Size() - 2 * RadiusOffSet;
		Direction = Direction.GetSafeNormal();

		FRotator LookatRot = UKismetMathLibrary::FindLookAtRotation(StartPos, EndPos);
		//LookatRot.Yaw = 0.f;

		FTransform NewTransform;
		NewTransform.SetLocation(StartPos + Direction * RadiusOffSet + FVector(0.f, 100.f, 0.f));
		NewTransform.SetScale3D(FVector(VectorLength*0.01f, 2.f, 0.2f));
		NewTransform.SetRotation(LookatRot.Quaternion());

		if (SplineMeshArray.IsValidIndex(TargetIdx - 1))
		{
			SplineMeshArray[TargetIdx - 1]->DestroyComponent();

			USplineMeshComponent* meshComponent = NewObject<USplineMeshComponent>(this);
			if (meshComponent)
			{
				meshComponent->OnComponentCreated();

				meshComponent->SetStaticMesh(WallStaticMesh);
				meshComponent->SetMaterial(0, WallMaterialDynamic);
				meshComponent->SetWorldTransform(NewTransform);

				meshComponent->SetForwardAxis(ESplineMeshAxis::Z);
				meshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

				meshComponent->RegisterComponent();
				SplineMeshArray[TargetIdx - 1] = meshComponent;
			}
		}
	}

	if (CachedPointArray.IsValidIndex(TargetIdx + 1)) // right
	{
		FVector StartPos = SplineComponent->GetLocationAtSplinePoint(TargetIdx, ESplineCoordinateSpace::World);
		FVector EndPos = SplineComponent->GetLocationAtSplinePoint(TargetIdx + 1, ESplineCoordinateSpace::World);

		//DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Blue, false, 1.f, 0, 3.f);

		FVector Direction = EndPos - StartPos;
		float VectorLength = Direction.Size() - 2 * RadiusOffSet;
		Direction = Direction.GetSafeNormal();

		FRotator LookatRot = UKismetMathLibrary::FindLookAtRotation(StartPos, EndPos);
		//LookatRot.Yaw = 0.f;
		FTransform NewTransform;
		NewTransform.SetLocation(StartPos + Direction * RadiusOffSet + FVector(0.f, 100.f, 0.f));
		NewTransform.SetScale3D(FVector(VectorLength*0.01f, 2.f, 0.2f));
		NewTransform.SetRotation(LookatRot.Quaternion());

		if (SplineMeshArray.IsValidIndex(TargetIdx))
		{
			SplineMeshArray[TargetIdx]->DestroyComponent();

			USplineMeshComponent* meshComponent = NewObject<USplineMeshComponent>(this);
			if (meshComponent)
			{
				meshComponent->OnComponentCreated();

				meshComponent->SetStaticMesh(WallStaticMesh);
				meshComponent->SetMaterial(0, WallMaterialDynamic);
				meshComponent->SetWorldTransform(NewTransform);

				meshComponent->SetForwardAxis(ESplineMeshAxis::Z);
				meshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

				meshComponent->RegisterComponent();
				SplineMeshArray[TargetIdx] = meshComponent;
			}
		}
	}
}

void ASplineWall::SetDisableExceptFor(class AWallColumn* Target)
{
	for (AWallColumn* Element : ColumnActorArray)
	{
		if (Element == Target)
			continue;

		Element->SetbPositionEditable(false);
	}
}

void ASplineWall::SetAllEnable(bool bEnable)
{
	for (AWallColumn* Element : ColumnActorArray)
	{
		Element->SetbPositionEditable(bEnable);
	}
}
