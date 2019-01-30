// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ProjectBallGameMode.h"
#include "Engine/DataTable.h"
#include "ProjectBallBall.h"
#include "TableInfos.h"
#include "TutorialPoint.h"
#include "ProjectBall.h"
#include "BallPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "BallGameInstance.h"

AProjectBallGameMode::AProjectBallGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AProjectBallBall::StaticClass();
}

void AProjectBallGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!BALLGAMEINSTANCE(this)->bTutorialCompleted)
	{
		SetCurrentMode(EGameModeState::TUTORIAL);
		SetCurrentTutorialMode(ETutorialMode::TUTO1);
	}
	else
	{
		SetCurrentMode(EGameModeState::IDLE);
		SetCurrentTutorialMode(ETutorialMode::TUTOEND);
	}

	
}

EGameModeState AProjectBallGameMode::GetCurrentMode()
{
	return CurrentMode;
}

void AProjectBallGameMode::SetCurrentMode(EGameModeState NewState)
{
	if (CurrentMode == NewState)
		return;

	CurrentMode = NewState;
	OnStateChanged.Broadcast(CurrentMode);
}

ETutorialMode AProjectBallGameMode::GetTutorialMode()
{
	return TutoMode;
}

void AProjectBallGameMode::SetCurrentTutorialMode(ETutorialMode NewMode)
{
	if (TutoMode == NewMode)
		return;

	TutoMode = NewMode;

	for (auto& Element : TutoPointArray)
		Element->Destroy();

	TutoPointArray.Empty();


	FTutorialPointTableInfo* tableinfo = TutorialPointTable->FindRow<FTutorialPointTableInfo>(FName(*FString::FromInt((int32)NewMode)), TEXT("GENERAL"), 0);
	if(tableinfo)
	{
		int32 idx = 0;
		for (auto& Element : tableinfo->Location)
		{
			ATutorialPoint* point = GetWorld()->SpawnActor<ATutorialPoint>(TutorialPointClass, Element, FRotator::ZeroRotator);
			if (point)
				TutoPointArray.Emplace(point);

			if (NewMode == ETutorialMode::TUTO3)
			{
				if (idx == 0 || idx == 2)
				{
					point->SetisActivatedAtLeastOnce(true);
				}
			}

			idx++;
		}

		if (tableinfo->bDeleteWall)
		{
			ABallPlayerController* PC = Cast<ABallPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
			if (PC)
			{
				PC->DestroyAllSplineWall();
			}
		}
	}


	OnTutoModeChanged.Broadcast(TutoMode);

	if (NewMode == ETutorialMode::TUTOEND)
	{
		ABallPlayerController* PC = Cast<ABallPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PC)
		{
			PC->DestroyAllSplineWall();
		}

		BALLGAMEINSTANCE(this)->bTutorialCompleted = true;

		SetCurrentMode(EGameModeState::IDLE);
	}
		
}

void AProjectBallGameMode::SkipTutorial()
{
	SetCurrentMode(EGameModeState::IDLE);
	SetCurrentTutorialMode(ETutorialMode::TUTOEND);
}

bool AProjectBallGameMode::isAllActivatedTutoPoint(TArray<FVector> SelectedPoints)
{
	bool bResult = true;
	for (auto& Element : TutoPointArray)
	{
		if (!Element->isActivatedAtLeastOnce())
			bResult = false;

		int32 FoundIdx = SelectedPoints.Find(Element->GetActorLocation());
		if (FoundIdx == INDEX_NONE)
			bResult = false;
	}

	return bResult;
}
