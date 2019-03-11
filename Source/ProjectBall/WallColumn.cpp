// Fill out your copyright notice in the Description page of Project Settings.

#include "WallColumn.h"
#include "SplineWall.h"

// Sets default values
AWallColumn::AWallColumn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWallColumn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWallColumn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWallColumn::SetbPositionEditable(bool bNewPositionEditable)
{
	PositionEditable(bNewPositionEditable);
}

void AWallColumn::EditPosition(FVector NewPos)
{
	SetActorLocation(NewPos);

	if (ParentWall.IsValid())
		ParentWall->PositionEdit(TargetIdx, NewPos);
}

void AWallColumn::MergePosition(FVector NewPos)
{
	if (ParentWall.IsValid())
		ParentWall->PositionMerge(TargetIdx);
}

void AWallColumn::SetCustomDepthRender(bool bRender)
{
	UStaticMeshComponent* StaticMeshComp = FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(bRender);
	}
}

void AWallColumn::GetNearColumn(TArray<AWallColumn*>& OutResult)
{
	if (ParentWall.IsValid())
	{
		TArray<AWallColumn*> Columns = ParentWall->GetColumnActorArray();
		int32 CurrentIdx = Columns.Find(this);
		if (CurrentIdx != INDEX_NONE)
		{
			if (Columns.IsValidIndex(CurrentIdx - 1))
			{
				OutResult.Emplace(Columns[CurrentIdx - 1]);
			}
			if (Columns.IsValidIndex(CurrentIdx + 1))
			{
				OutResult.Emplace(Columns[CurrentIdx + 1]);
			}
		}
	}
	
}

bool AWallColumn::isTailColumn()
{
	bool isTail = ParentWall->isTailColumn(this);
	return isTail;
}

bool AWallColumn::isEdgeColumn()
{
	return isHeadColumn() || isTailColumn();
}

bool AWallColumn::isHeadColumn()
{
	bool isHead = ParentWall->isHeadColumn(this);
	return isHead;
}

