// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialPoint.h"

// Sets default values
ATutorialPoint::ATutorialPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATutorialPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATutorialPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

