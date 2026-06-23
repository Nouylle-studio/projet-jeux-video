// Fill out your copyright notice in the Description page of Project Settings.

#include "TramTrack.h"

ATramTrack::ATramTrack()
{
	PrimaryActorTick.bCanEverTick = false;

	TramSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TramSpline"));
	RootComponent = TramSpline;
}

void ATramTrack::BeginPlay()
{
	Super::BeginPlay();
}

