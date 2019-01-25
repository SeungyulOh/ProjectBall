// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ProjectBallGameMode.h"
#include "ProjectBallBall.h"

AProjectBallGameMode::AProjectBallGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AProjectBallBall::StaticClass();
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
