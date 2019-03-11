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
#include "StageHelper.h"

AProjectBallGameMode::AProjectBallGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AProjectBallBall::StaticClass();
}

void AProjectBallGameMode::BeginPlay()
{
	Super::BeginPlay();

	static int32 uuid = 0;
	FLatentActionInfo info;
	info.Linkage = 0;
	info.UUID = ++uuid;
	info.CallbackTarget = this;
	info.ExecutionFunction = TEXT("Callback_SublevelLoaded");
	int32 CurrentStage = UStageHelper::Get()->GetCurrentStage();
	FString MapName = TEXT("Level") + FString::FromInt(CurrentStage);
	UGameplayStatics::LoadStreamLevel(this, FName(*MapName), true, true, info);

	
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
			else if (NewMode == ETutorialMode::TUTO4)
			{
				if (idx != 3)
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

void AProjectBallGameMode::AddCurrentStage()
{
	UStageHelper::Get()->AddCurrentStage();
	
	BALLGAMEINSTANCE(this)->WallMemoryHelper->Clear();
}


int32 AProjectBallGameMode::GetCurrentStage()
{
	return UStageHelper::Get()->GetCurrentStage();
}

void AProjectBallGameMode::Callback_SublevelLoaded()
{
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

void AProjectBallGameMode::SetStage(int32 NewStageidx)
{
	UStageHelper::Get()->SetCurrentStage(NewStageidx);
	UGameplayStatics::OpenLevel(this, TEXT("StageMap"));
}
