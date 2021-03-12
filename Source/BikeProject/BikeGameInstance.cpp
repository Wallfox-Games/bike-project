// Fill out your copyright notice in the Description page of Project Settings.


#include "BikeGameInstance.h"

void UBikeGameInstance::Init()
{
	Task = new BikePhysicalInput(&SpeedsArray);
	Task->Run();
}
