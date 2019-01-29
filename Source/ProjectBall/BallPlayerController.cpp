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
		TArray<FHitResult> outResult;
		GetWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 99999.f, ECollisionChannel::ECC_WorldStatic);
		for (size_t i = 0; i < outResult.Num(); ++i)
		{
			if (BallGameMode->GetCurrentMode() == EGameModeState::TUTORIAL)
			{
				if (outResult[i].Actor->GetName().Contains(TEXT("Tuto")))
				{
					if (PointArray.Num() == 0)
					{
						ATutorialPoint* TutoPoint = Cast<ATutorialPoint>(outResult[i].Actor);
						if (TutoPoint)
							TutoPoint->ChangeMaterialColor(false);

						ASplineWall* Wall = GetWorld()->SpawnActor<ASplineWall>(SplineWallClass);
						PointArray.AddUnique(outResult[i].Actor->GetActorLocation());
						SplineWallArray.Emplace(Wall);
					}

					PointNum = PointArray.Num();
				}
			}
			else if (outResult[i].Actor->GetName().Contains(TEXT("Floor")))
			{
				if (PointArray.Num() == 0)
				{
					ASplineWall* Wall = GetWorld()->SpawnActor<ASplineWall>(SplineWallClass);
					PointArray.AddUnique(outResult[i].ImpactPoint);
					SplineWallArray.Emplace(Wall);
				}

				PointNum = PointArray.Num();
				break;
			}
			else if (outResult[i].Actor->GetName().Contains(TEXT("Column")))
			{
				PositionEditingWallColumn = Cast<AWallColumn>(outResult[i].Actor);
				PositionEditingWallColumn->SetActorEnableCollision(false);

				bEnable = true;
				DepthEffectElapsedTime = 0.f;
			}
			else
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, outResult[i].Actor->GetName());
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
		TArray<FHitResult> outResult;
		GetWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 99999.f, ECollisionChannel::ECC_WorldStatic);
		for (size_t i = 0; i < outResult.Num(); ++i)
		{
			if (BallGameMode->GetCurrentMode() == EGameModeState::TUTORIAL)
			{
				if (outResult[i].Actor->GetName().Contains(TEXT("Tuto")))
				{
					if (PointArray.Num() >= PointNum + 1)
						PointArray.Pop();

					ATutorialPoint* TutoPoint = Cast<ATutorialPoint>(outResult[i].Actor);
					if (TutoPoint)
					{
						LastTouchedTutoPoint = TutoPoint;
						TutoPoint->ChangeMaterialColor(false);
					}
						

					PointArray.AddUnique(outResult[i].Actor->GetActorLocation());
					OnSelectedPoint.ExecuteIfBound(PointArray);
				}
				else if (outResult[i].Actor->GetName().Contains(TEXT("Floor")))
				{
					if (LastTouchedTutoPoint.IsValid())
					{
						LastTouchedTutoPoint->ChangeMaterialColor(true);
						LastTouchedTutoPoint = nullptr;
					}

					if (PointArray.Num() >= PointNum + 1)
						PointArray.Pop();

					PointArray.AddUnique(outResult[i].ImpactPoint);
					OnSelectedPoint.ExecuteIfBound(PointArray);
					break;
				}
			}
			else if (PositionEditingWallColumn.IsValid())
			{
				PositionEditingWallColumn->bPositionMergable = false;
				PositionEditingWallColumn->SetCustomDepthRender(false);

				if (outResult[i].Actor->GetName().Contains(TEXT("Floor")))
					PositionEditingWallColumn->EditPosition(outResult[i].ImpactPoint);
				else if (outResult[i].Actor->GetName().Contains(TEXT("Col")))
				{
					AWallColumn* Target = Cast<AWallColumn>(outResult[i].Actor);
					if (IsValid(Target))
					{
						int32 diff = Target->TargetIdx - PositionEditingWallColumn->TargetIdx;
						if (FMath::Abs<int32>(diff) == 1 && PositionEditingWallColumn->ParentWall.Get() == Target->ParentWall)
						{
							PositionEditingWallColumn->SetCustomDepthRender(true);
							PositionEditingWallColumn->EditPosition(outResult[i].Actor->GetActorLocation());
							PositionEditingWallColumn->bPositionMergable = true;
						}
					}
				}
				else
					GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, outResult[i].Actor->GetName());
			}
			else if (outResult[i].Actor->GetName().Contains(TEXT("Floor")))
			{
				if (PointArray.Num() >= PointNum + 1)
					PointArray.Pop();

				PointArray.AddUnique(outResult[i].ImpactPoint);
				OnSelectedPoint.ExecuteIfBound(PointArray);
				break;
			}
		}
	}
}

void ABallPlayerController::CallbackInputTouchEnd(ETouchIndex::Type TouchIndex, FVector Location)
{
	if (CurrentTouchType == ETouchIndex::MAX_TOUCHES ||
		CurrentTouchType != TouchIndex)
		return;

	/*FString str = TEXT("CallbackInputTouchEnd idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, str);*/
	//tutorial
	AProjectBallGameMode* BallGameMode = Cast<AProjectBallGameMode>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(BallGameMode))
		return;

	if (BallGameMode->GetCurrentMode() == EGameModeState::TUTORIAL)
	{
		if (LastTouchedTutoPoint.IsValid())
		{
			LastTouchedTutoPoint->ChangeMaterialColor(true);
			BallGameMode->SetCurrentTutorialMode(ETutorialMode::TUTO2);
		}
		else
		{
			TWeakObjectPtr<ASplineWall> TargetWall = SplineWallArray.Pop();
			if (TargetWall.IsValid())
				TargetWall->DestroyAll();
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

	PointArray.Empty();
	OnFinishedWall.ExecuteIfBound();

	CurrentTouchType = ETouchIndex::MAX_TOUCHES;
	
}


void ABallPlayerController::CallbackInputTouch2()
{
	CallbackInputTouchBegin(ETouchIndex::Touch2, FVector(300, 300, 1));
}

void ABallPlayerController::PlayStart()
{
	for (auto& Element : SplineWallArray)
	{
		if (Element.IsValid())
		{
			Element->SetAllDisablePositionEdit();
		}
	}

	DisableInput(this);
}

