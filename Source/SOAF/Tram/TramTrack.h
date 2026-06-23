// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "TramTrack.generated.h"

UCLASS()
class SOAF_API ATramTrack : public AActor
{
	GENERATED_BODY()

public:
	ATramTrack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tram")
	USplineComponent* TramSpline;

protected:
	virtual void BeginPlay() override;

};
