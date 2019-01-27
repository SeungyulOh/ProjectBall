// Fill out your copyright notice in the Description page of Project Settings.

#include "BallPlayerController.h"
#include "Engine.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "SplineWall.h"
#include "WallColumn.h"

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
	InputComponent->BindTouch(IE_DoubleClick, this, &ABallPlayerController::CallbackInputDoubleTouch);

	InputComponent->BindAction(TEXT("Touch2"), EInputEvent::IE_Pressed, this, &ABallPlayerController::CallbackInputTouch2);

}

void ABallPlayerController::CallbackInputTouchBegin(ETouchIndex::Type TouchIndex, FVector Location)
{
#if WITH_EDITOR
	FString str = TEXT("CallbackInputTouchBegin idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	UE_LOG(LogClass, Log, TEXT("%s"), *str);
#endif

	/*FString str2 = TEXT("CallbackInputTouchOver idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, str2);*/

	CurrentTouchType = TouchIndex;

	/*float viewScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));*/

	FVector WorldLocation;
	FVector WorldDirection;
	if (DeprojectScreenPositionToWorld(Location.X, Location.Y, WorldLocation, WorldDirection))
	{
		TArray<FHitResult> outResult;
		GetWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 99999.f, ECollisionChannel::ECC_WorldStatic);
		for (size_t i = 0; i < outResult.Num(); ++i)
		{
			if (outResult[i].Actor->GetName().Contains(TEXT("Floor")))
			{
				if (PointArray.Num() == 0)
				{
					ASplineWall* Wall = GetWorld()->SpawnActor<ASplineWall>(SplineWallClass);
					PointArray.AddUnique(outResult[i].ImpactPoint);
				}

				PointNum = PointArray.Num();
				break;
			}
			else if (outResult[i].Actor->GetName().Contains(TEXT("Column")))
			{
				PositionEditingWallColumn = Cast<AWallColumn>(outResult[i].Actor);
				PositionEditingWallColumn->SetActorEnableCollision(false);
				if (PositionEditingWallColumn->ParentWall.IsValid())
				{
					PositionEditingWallColumn->ParentWall->SetDisableExceptFor(PositionEditingWallColumn.Get());
				}
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

	FVector WorldLocation;
	FVector WorldDirection;
	if (DeprojectScreenPositionToWorld(Location.X, Location.Y, WorldLocation, WorldDirection))
	{
		TArray<FHitResult> outResult;
		GetWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 99999.f, ECollisionChannel::ECC_WorldStatic);
		for (size_t i = 0; i < outResult.Num(); ++i)
		{
			if (PositionEditingWallColumn.IsValid())
			{
				if (outResult[i].Actor->GetName().Contains(TEXT("Floor")))
					PositionEditingWallColumn->EditPosition(outResult[i].ImpactPoint);
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
	
	if (PositionEditingWallColumn.IsValid())
	{
		PositionEditingWallColumn->SetActorEnableCollision(true);
		if (PositionEditingWallColumn->ParentWall.IsValid())
		{
			PositionEditingWallColumn->ParentWall->SetAllEnable(true);
		}

		PositionEditingWallColumn = nullptr;
	}

	PointArray.Empty();
	OnFinishedWall.ExecuteIfBound();

	CurrentTouchType = ETouchIndex::MAX_TOUCHES;
	
}

void ABallPlayerController::CallbackInputDoubleTouch(ETouchIndex::Type TouchIndex, FVector Location)
{
	
}

void ABallPlayerController::CallbackInputTouch2()
{
	CallbackInputTouchBegin(ETouchIndex::Touch2, FVector(300, 300, 1));
}
