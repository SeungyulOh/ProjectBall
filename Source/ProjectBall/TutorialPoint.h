// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialPoint.generated.h"

UCLASS()
class PROJECTBALL_API ATutorialPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ChangeMaterialColor(bool bOriginalColor);

	FORCEINLINE bool isActivatedAtLeastOnce() {		return bActivatedAtLeastOne;	}
	FORCEINLINE void SetisActivatedAtLeastOnce(bool InBool) { bActivatedAtLeastOne = InBool; }

private:
	bool bActivatedAtLeastOne = false;

};
