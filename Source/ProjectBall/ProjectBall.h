// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UMG.h"
#include "Engine.h"
#include "UnrealEngine.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

#include "BallGameInstance.h"

extern PROJECTBALL_API class UBallGameInstance*	BallGameInstance;

#if WITH_EDITOR
#define BALLGAMEINSTANCE(p) (UBallGameInstance::Get(p))
#else
#define BALLGAMEINSTANCE(p) BallGameInstance
#endif