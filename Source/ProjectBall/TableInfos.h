// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TableInfos.generated.h"

/**
 * 
 */

USTRUCT()
struct FTutorialPointTableInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FTutorialPointTableInfo")
	TArray<FVector> Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FTutorialPointTableInfo")
	bool		bDeleteWall;

};