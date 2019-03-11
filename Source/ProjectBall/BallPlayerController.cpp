// Fill out your copyright notice in the Description page of Project Settings.

#include "BallPlayerController.h"
#include "Engine.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "SplineWall.h"
#include "WallColumn.h"
#include "Engine/PostProcessVolume.h"
#include "ProjectBallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TutorialPoint.h"
#include "ProjectBall.h"
#include "BallGameInstance.h"
#include "HeapSort.h"

ABallPlayerController::ABallPlayerController()
{
	bShowMouseCursor = true;
}

void ABallPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindTouch(IE_Pressed, this, &ABallPlayerController::CallbackInputTouchBegin);
	InputComponent->BindTouch(IE_Repeat, this, &ABallPlayerController::CallbackInputTouchOver);
	InputComponent->BindTouch(IE_Released, this, &ABallPlayerController::CallbackInputTouchEnd);

	InputComponent->BindAction(TEXT("Touch2"), EInputEvent::IE_Pressed, this, &ABallPlayerController::CallbackInputTouch2);

}

void ABallPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, APostProcessVolume::StaticClass(), OutActors);


	for (auto& Element : GetWorld()->PostProcessVolumes)
	{
		APostProcessVolume* PPVolume = Cast<APostProcessVolume>(Element);
		if (PPVolume)
		{
			if (PPVolume->Settings.WeightedBlendables.Array.IsValidIndex(0))
			{
				UMaterialInterface* OutLineMat = Cast<UMaterialInterface>(PPVolume->Settings.WeightedBlendables.Array[0].Object);
				if (OutLineMat)
				{
					MID = UMaterialInstanceDynamic::Create(OutLineMat, nullptr);
					PPVolume->Settings.WeightedBlendables.Array[0].Object = MID;
					break;
				}
			}
		}
	}

	if (BALLGAMEINSTANCE(this)->WallMemoryHelper->isWallMemoryExist())
	{
		BALLGAMEINSTANCE(this)->WallMemoryHelper->MakeWall(this);
	}
}

void ABallPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DepthEffectElapsedTime >= 0 && DepthEffectElapsedTime <= DepthEffectDuration)
	{
		DepthEffectElapsedTime += DeltaSeconds * (bEnable ? 1 : -1);

		float Value = (DepthEffectDuration - DepthEffectElapsedTime) / DepthEffectDuration;
		Value = FMath::Clamp<float>(Value, 0.25f, 1.f);

		if (IsValid(MID))
			MID->SetScalarParameterValue(TEXT("DarkIntensity"), Value);
	}
}



void ABallPlayerController::CallbackInputTouchBegin(ETouchIndex::Type TouchIndex, FVector Location)
{
#if WITH_EDITOR
	FString str = TEXT("CallbackInputTouchBegin idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	UE_LOG(LogClass, Log, TEXT("%s"), *str);
#endif

	/*FString str2 = TEXT("CallbackInputTouchOver idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, str2);*/

	if(TouchIndex != ETouchIndex::MAX_TOUCHES)
		CurrentTouchType = ETouchIndex::Touch1;

	//tutorial
	AProjectBallGameMode* BallGameMode = Cast<AProjectBallGameMode>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(BallGameMode))
		return;
	FVector WorldLocation;
	FVector WorldDirection;
	if (DeprojectScreenPositionToWorld(Location.X, Location.Y, WorldLocation, WorldDirection))
	{
		FHitResult outResult;
		GetWorld()->LineTraceSingleByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 99999.f, ECollisionChannel::ECC_WorldStatic);
		if (BallGameMode->GetCurrentMode() == EGameModeState::TUTORIAL)
		{
			if (outResult.Actor->GetName().Contains(TEXT("Tuto")))
			{
				if (PointArray.Num() == 0)
				{
					ATutorialPoint* TutoPoint = Cast<ATutorialPoint>(outResult.Actor);
					if (TutoPoint)
					{
						TutoPoint->SetisActivatedAtLeastOnce(true);
						TutoPoint->ChangeMaterialColor(false);
					}
							
					if (BallGameMode->GetTutorialMode() <= ETutorialMode::TUTO2 ||
						BallGameMode->GetTutorialMode() == ETutorialMode::TUTO4)
					{
						ASplineWall* Wall = GetWorld()->SpawnActor<ASplineWall>(SplineWallClass);
						PointArray.AddUnique(outResult.Actor->GetActorLocation());
						SplineWallArray.Emplace(Wall);
					}
				}

				for (size_t i = 0; i < SplineWallArray.Num(); ++i)
				{
					if (i == SplineWallArray.Num() - 1)
						break;

					if (IsValid(SplineWallArray[i]))
					{
						SplineWallArray[i]->SetAllEnablePositionEdit(false);
						SplineWallArray[i]->SetEnableEdgeMerge(true);
					}
				}
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Column")))
			{
				if (BallGameMode->GetTutorialMode() == ETutorialMode::TUTO4)
					return;

				PositionEditingWallColumn = Cast<AWallColumn>(outResult.Actor);
				PositionEditingWallColumn->SetActorEnableCollision(false);

				for (auto& Element : SplineWallArray)
				{
					if (IsValid(Element))
					{
						Element->SetAllEnablePositionEdit(false);
					}
				}
				TArray<AWallColumn*> TargetColumns;
				PositionEditingWallColumn->GetNearColumn(TargetColumns);
				for (auto& Element : TargetColumns)
				{
					Element->SetbPositionEditable(true);
				}
				PositionEditingWallColumn->SetbPositionEditable(true);

				bEnable = true;
				DepthEffectElapsedTime = 0.f;
			}
			PointNum = PointArray.Num();
		}
		else
		{
			if (outResult.Actor->GetName().Contains(TEXT("Floor")))
			{
				if (PointArray.Num() == 0)
				{
					SpawnSplineWall();

					PointArray.AddUnique(outResult.ImpactPoint);
				}

					
				for (size_t i = 0 ; i < SplineWallArray.Num() ; ++i)
				{
					if (i == SplineWallArray.Num() - 1)
						break;

					if (IsValid(SplineWallArray[i]))
					{
						SplineWallArray[i]->SetAllEnablePositionEdit(false);
						SplineWallArray[i]->SetEnableEdgeMerge(true);
					}
				}

				PointNum = PointArray.Num();
				FString str = FString::FromInt(PointNum);
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, str);
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Column")))
			{
				PositionEditingWallColumn = Cast<AWallColumn>(outResult.Actor);
				PositionEditingWallColumn->SetActorEnableCollision(false);

				for (auto& Element : SplineWallArray)
				{
					if (IsValid(Element))
					{
						Element->SetAllEnablePositionEdit(false);
					}
				}
				TArray<AWallColumn*> TargetColumns;
				PositionEditingWallColumn->GetNearColumn(TargetColumns);
				for (auto& Element : TargetColumns)
				{
					Element->SetbPositionEditable(true);
				}
				PositionEditingWallColumn->SetbPositionEditable(true);

				bEnable = true;
				DepthEffectElapsedTime = 0.f;
			}
			else
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, outResult.Actor->GetName());
		}
	}
	
}

void ABallPlayerController::CallbackInputTouchOver(ETouchIndex::Type TouchIndex, FVector Location)
{
	if (CurrentTouchType == ETouchIndex::MAX_TOUCHES ||
		CurrentTouchType != TouchIndex)
		return;

	/*FString str = TEXT("CallbackInputTouchOver idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" CurrentTouchType : ") + FString::FromInt((int)CurrentTouchType);
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, str);*/

	//tutorial
	AProjectBallGameMode* BallGameMode = Cast<AProjectBallGameMode>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(BallGameMode))
		return;


	FVector WorldLocation;
	FVector WorldDirection;
	if (DeprojectScreenPositionToWorld(Location.X, Location.Y, WorldLocation, WorldDirection))
	{
		FHitResult outResult;
		GetWorld()->LineTraceSingleByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 99999.f, ECollisionChannel::ECC_WorldStatic);
		if (BallGameMode->GetCurrentMode() == EGameModeState::TUTORIAL)
		{
			if (PositionEditingWallColumn.IsValid())
			{
				PositionEditingWallColumn->bPositionMergable = false;
				PositionEditingWallColumn->SetCustomDepthRender(false);

				if (outResult.Actor->GetName().Contains(TEXT("Floor")))
				{
					if (LastTouchedTutoPoint.IsValid())
					{
						LastTouchedTutoPoint->ChangeMaterialColor(true);
						LastTouchedTutoPoint = nullptr;
					}

					PositionEditingWallColumn->EditPosition(outResult.ImpactPoint);
				}
				else if (outResult.Actor->GetName().Contains(TEXT("Tuto")))
				{
					ATutorialPoint* TutoPoint = Cast<ATutorialPoint>(outResult.Actor);
					if (TutoPoint)
					{
						LastTouchedTutoPoint = TutoPoint;
						LastTouchedTutoPoint->ChangeMaterialColor(false);
						LastTouchedTutoPoint->SetisActivatedAtLeastOnce(true);
					}

					PositionEditingWallColumn->EditPosition(outResult.Actor->GetActorLocation());
				}
				else if (outResult.Actor->GetName().Contains(TEXT("Col")))
				{
					AWallColumn* Target = Cast<AWallColumn>(outResult.Actor);
					if (IsValid(Target))
					{
						int32 diff = Target->TargetIdx - PositionEditingWallColumn->TargetIdx;
						if (FMath::Abs<int32>(diff) == 1 && PositionEditingWallColumn->ParentWall.Get() == Target->ParentWall)
						{
							PositionEditingWallColumn->SetCustomDepthRender(true);
							PositionEditingWallColumn->EditPosition(outResult.Actor->GetActorLocation());
							PositionEditingWallColumn->bPositionMergable = true;
						}
					}
				}
				else
					GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, outResult.Actor->GetName());
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Col")))
			{
				int32 MaxIdx = SplineWallArray.Num() - 1;
				if (IsValid(SplineWallArray[MaxIdx]))
				{
					AWallColumn* WallColumn = Cast<AWallColumn>(outResult.GetActor());
					if (IsValid(WallColumn) && WallColumn->isEdgeColumn())
					{
						if (WallColumn->ParentWall.IsValid())
						{
							int32 FoundIdx = SplineWallArray.Find(WallColumn->ParentWall.Get());
							if (FoundIdx != INDEX_NONE && FoundIdx != MaxIdx)
							{
								if (PointArray.Num() >= PointNum + 1)
								{
									PointArray.Pop();
								}
								PointArray.AddUnique(WallColumn->GetActorLocation());
								OnSelectedPoint.ExecuteIfBound(PointArray);

								if (WallColumn->isHeadColumn())
								{
									MergeWaitingList.AddUnique(SplineWallArray[MaxIdx]);
									MergeWaitingList.AddUnique(WallColumn->ParentWall.Get());
								}
								else if (WallColumn->isTailColumn())
								{
									MergeWaitingList.AddUnique(WallColumn->ParentWall.Get());
									MergeWaitingList.AddUnique(SplineWallArray[MaxIdx]);
								}
								
							}
						}
					}
				}
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Tuto")))
			{
				ATutorialPoint* TutoPoint = Cast<ATutorialPoint>(outResult.Actor);
				if (TutoPoint)
				{
					LastTouchedTutoPoint = TutoPoint;
					LastTouchedTutoPoint->ChangeMaterialColor(false);
					LastTouchedTutoPoint->SetisActivatedAtLeastOnce(true);
				}

				if (PointArray.Num() >= PointNum + 1)
					PointArray.Pop();

				PointArray.AddUnique(outResult.Actor->GetActorLocation());
				OnSelectedPoint.ExecuteIfBound(PointArray);
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Floor")))
			{
				if (LastTouchedTutoPoint.IsValid())
				{
					LastTouchedTutoPoint->ChangeMaterialColor(true);
					LastTouchedTutoPoint = nullptr;
				}

				if (PointArray.Num() >= PointNum + 1)
					PointArray.Pop();

				PointArray.AddUnique(outResult.ImpactPoint);
				OnSelectedPoint.ExecuteIfBound(PointArray);
			}
		}
		else
		{
			isThumbOnGoalPoint = false;
			if (PositionEditingWallColumn.IsValid())
			{
				PositionEditingWallColumn->bPositionMergable = false;
				PositionEditingWallColumn->SetCustomDepthRender(false);

				if (outResult.Actor->GetName().Contains(TEXT("Floor")))
				{
					PositionEditingWallColumn->EditPosition(outResult.ImpactPoint);
				}
				else if (outResult.Actor->GetName().Contains(TEXT("Col")))
				{
					AWallColumn* Target = Cast<AWallColumn>(outResult.Actor);
					if (IsValid(Target))
					{
						int32 diff = Target->TargetIdx - PositionEditingWallColumn->TargetIdx;
						if (FMath::Abs<int32>(diff) == 1 && PositionEditingWallColumn->ParentWall.Get() == Target->ParentWall)
						{
							PositionEditingWallColumn->SetCustomDepthRender(true);
							PositionEditingWallColumn->EditPosition(outResult.Actor->GetActorLocation());
							PositionEditingWallColumn->bPositionMergable = true;
						}
					}
				}
				else
				{
					if (outResult.Actor->GetName().Contains(TEXT("Goal")))
					{
						isThumbOnGoalPoint = true;
						GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, outResult.Actor->GetName());
					}
							
				}
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Floor")))
			{
				if (PointArray.Num() >= PointNum + 1)
				{
					PointArray.Pop();
				}
				PointArray.AddUnique(outResult.ImpactPoint);
				OnSelectedPoint.ExecuteIfBound(PointArray);
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Col")))
			{
				int32 MaxIdx = SplineWallArray.Num() - 1;
				if (IsValid(SplineWallArray[MaxIdx]))
				{
					AWallColumn* WallColumn = Cast<AWallColumn>(outResult.GetActor());
					if (IsValid(WallColumn) && WallColumn->isEdgeColumn())
					{
						if (WallColumn->ParentWall.IsValid())
						{
							int32 FoundIdx = SplineWallArray.Find(WallColumn->ParentWall.Get());
							if (FoundIdx != INDEX_NONE && FoundIdx != MaxIdx)
							{
								if (PointArray.Num() >= PointNum + 1)
								{
									PointArray.Pop();
								}
								PointArray.AddUnique(WallColumn->GetActorLocation());
								OnSelectedPoint.ExecuteIfBound(PointArray);

								if (WallColumn->isHeadColumn())
								{
									MergeWaitingList.AddUnique(SplineWallArray[MaxIdx]);
									MergeWaitingList.AddUnique(WallColumn->ParentWall.Get());
								}
								else if (WallColumn->isTailColumn())
								{
									MergeWaitingList.AddUnique(WallColumn->ParentWall.Get());
									MergeWaitingList.AddUnique(SplineWallArray[MaxIdx]);
								}
							}
						}
					}
				}
			}
			else if (outResult.Actor->GetName().Contains(TEXT("Goal")) && PointArray.Num())
			{
				isThumbOnGoalPoint = true;
			}
		}
	}
}

void ABallPlayerController::CallbackInputTouchEnd(ETouchIndex::Type TouchIndex, FVector Location)
{
	if (CurrentTouchType == ETouchIndex::MAX_TOUCHES ||
		CurrentTouchType != TouchIndex)
		return;

	//tutorial
	AProjectBallGameMode* BallGameMode = Cast<AProjectBallGameMode>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(BallGameMode))
		return;

	

	

	for (auto& Element : SplineWallArray)
	{
		if (IsValid(Element))
		{
			Element->SetAllEnablePositionEdit(true);
		}
	}

	isThumbOnGoalPoint = false;
	PointArray.Empty();

	if (MergeWaitingList.Num())
	{
		TArray<FVector> Points;
		int32 idx = 0;
		for (auto& Element : MergeWaitingList)
		{
			TArray<FVector> CachedPoints = Element->GetCachedPointArray();

			if (idx == 1)
			{
				if (CachedPoints[CachedPoints.Num() - 1] == Points[Points.Num() - 1])
				{
					Algo::Reverse(CachedPoints);
				}
			}
			
			for (size_t i = 0; i < CachedPoints.Num(); ++i)
			{
				Points.AddUnique(CachedPoints[i]);
			}
			Element->DestroyAll();
			++idx;
		}

		ASplineWall* Wall = SpawnSplineWall();
		if (IsValid(Wall))
		{
			Wall->MakeWall(Points);
			SplineWallArray.AddUnique(Wall);
		}

		MergeWaitingList.Empty();
	}
	else
		OnFinishedWall.ExecuteIfBound();



	if (BallGameMode->GetCurrentMode() == EGameModeState::TUTORIAL)
	{
		if (LastTouchedTutoPoint.IsValid())
			LastTouchedTutoPoint->ChangeMaterialColor(true);

		if (SplineWallArray.IsValidIndex(0))
		{
			TArray<FVector> points = SplineWallArray[0]->GetCachedPointArray();

			ETutorialMode mode = BallGameMode->GetTutorialMode();
			if (mode <= ETutorialMode::TUTO2)
			{
				if (BallGameMode->isAllActivatedTutoPoint(points))
				{
					int32 Idx = (int32)mode;
					Idx++;
					BallGameMode->SetCurrentTutorialMode((ETutorialMode)Idx);
				}
				else
				{
					if (SplineWallArray.Num())
					{
						TWeakObjectPtr<ASplineWall> TargetWall = SplineWallArray.Pop();
						if (TargetWall.IsValid())
							TargetWall->DestroyAll();
					}
				}
			}
			else if (mode == ETutorialMode::TUTO3)
			{
				if (BallGameMode->isAllActivatedTutoPoint(points))
				{
					int32 Idx = (int32)mode;
					Idx++;
					BallGameMode->SetCurrentTutorialMode((ETutorialMode)Idx);
				}
				else
				{
					if (PositionEditingWallColumn.IsValid())
					{
						PositionEditingWallColumn->SetActorEnableCollision(true);
						PositionEditingWallColumn = nullptr;

						bEnable = false;
						if (DepthEffectElapsedTime > DepthEffectDuration)
							DepthEffectElapsedTime = DepthEffectDuration;
					}
				}
			}
			else if (mode == ETutorialMode::TUTO4)
			{
				if (BallGameMode->isAllActivatedTutoPoint(points))
				{
					int32 Idx = (int32)mode;
					Idx++;
					BallGameMode->SetCurrentTutorialMode((ETutorialMode)Idx);
				}
				else
				{
					if (SplineWallArray.Num() == 2)
					{
						TWeakObjectPtr<ASplineWall> TargetWall = SplineWallArray.Pop();
						if (TargetWall.IsValid())
							TargetWall->DestroyAll();
					}
				}
			}
			else if (mode == ETutorialMode::TUTO5)
			{
				if (PositionEditingWallColumn.IsValid() && PositionEditingWallColumn->bPositionMergable &&
					PositionEditingWallColumn->ParentWall->GetCachedPointArray().Num() == 2)
				{
					int32 Idx = (int32)mode;
					Idx++;
					BallGameMode->SetCurrentTutorialMode((ETutorialMode)Idx);

					bEnable = false;
					if (DepthEffectElapsedTime > DepthEffectDuration)
						DepthEffectElapsedTime = DepthEffectDuration;
				}
			}
		}
	}

	if (PositionEditingWallColumn.IsValid())
	{
		PositionEditingWallColumn->SetActorEnableCollision(true);

		if (PositionEditingWallColumn->bPositionMergable)
		{
			PositionEditingWallColumn->MergePosition(PositionEditingWallColumn->GetActorLocation());
		}


		PositionEditingWallColumn = nullptr;

		bEnable = false;
		if (DepthEffectElapsedTime > DepthEffectDuration)
			DepthEffectElapsedTime = DepthEffectDuration;
	}

	CurrentTouchType = ETouchIndex::MAX_TOUCHES;
	
}


void ABallPlayerController::CallbackInputTouch2()
{
	CallbackInputTouchBegin(ETouchIndex::Touch2, FVector(0, 0, 1));
}

void ABallPlayerController::PlayStart()
{
	for (auto& Element : SplineWallArray)
	{
		if (IsValid(Element))
		{
			Element->SetAllEnablePositionEdit(false);
		}
	}

	DisableInput(this);
}

void ABallPlayerController::SaveWallPoints()
{
	TArray<FPointVectors> Points;

	for (size_t i = 0; i < SplineWallArray.Num(); ++i)
	{
		FPointVectors vectors;
		if (IsValid(SplineWallArray[i]))
		{
			TArray<FVector> cachedPoints = SplineWallArray[i]->GetCachedPointArray();
			vectors.Points = cachedPoints;
			Points.Emplace(vectors);
		}
	}

	BALLGAMEINSTANCE(this)->WallMemoryHelper->SavePointVectors(Points);
}

void ABallPlayerController::DestroyAllSplineWall()
{
	if (SplineWallArray.Num() == 0)
		return;

	for (size_t i = 0 ; i<SplineWallArray.Num() ; )
	{
		if (IsValid(SplineWallArray[i]))
		{
			SplineWallArray[i]->DestroyAll();
			continue;
		}
		
		break;
	}

	SplineWallArray.Empty();
}

void ABallPlayerController::RemoveElementInVector(ASplineWall* InWall)
{
	int32 FoundIdx = SplineWallArray.Find(InWall);
	if (FoundIdx != INDEX_NONE)
	{
		SplineWallArray.RemoveAt(FoundIdx);
	}
}

ASplineWall* ABallPlayerController::SpawnSplineWall()
{
	ASplineWall* Wall = GetWorld()->SpawnActor<ASplineWall>(SplineWallClass);
	SplineWallArray.Emplace(Wall);

	return Wall;
}

