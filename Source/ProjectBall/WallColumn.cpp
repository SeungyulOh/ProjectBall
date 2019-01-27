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
	bPositionEditable = bNewPositionEditable;
	PositionEditable(bPositionEditable);
}

void AWallColumn::EditPosition(FVector NewPos)
{
	SetActorLocation(NewPos);

	if (ParentWall.IsValid())
		ParentWall->PositionEdit(TargetIdx, NewPos);
}
