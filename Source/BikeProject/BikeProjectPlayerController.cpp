// Fill out your copyright notice in the Description page of Project Settings.


#include "BikeProjectPlayerController.h"

void ABikeProjectPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameAndUI());
}