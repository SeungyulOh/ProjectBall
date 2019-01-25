// Fill out your copyright notice in the Description page of Project Settings.

#include "BallPlayerController.h"
#include "Engine.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"

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

}

void ABallPlayerController::CallbackInputTouchBegin(ETouchIndex::Type TouchIndex, FVector Location)
{
#if WITH_EDITOR
	FString str = TEXT("CallbackInputTouchBegin idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	UE_LOG(LogClass, Log, TEXT("%s"), *str);
#endif

	if (CurrentTouchType == ETouchIndex::MAX_TOUCHES)
	{
		CurrentTouchType = TouchIndex;
		StartPos = FVector2D(Location.X, Location.Y);
		CurrentPos = StartPos;
	}

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
				DrawDebugBox(GetWorld(), outResult[i].ImpactPoint, FVector(20.f, 20.f, 20.f), FColor::Red, false, 10.f, 0, 10.f);
				PointArray.AddUnique(outResult[i].ImpactPoint);
				OnSelectedPoint.ExecuteIfBound(PointArray);
				break;
			}
		}
	}
	
}

void ABallPlayerController::CallbackInputTouchOver(ETouchIndex::Type TouchIndex, FVector Location)
{

}

void ABallPlayerController::CallbackInputTouchEnd(ETouchIndex::Type TouchIndex, FVector Location)
{

}
