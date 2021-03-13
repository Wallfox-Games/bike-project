// Fill out your copyright notice in the Description page of Project Settings.


#include "BikeGameInstance.h"

void UBikeGameInstance::Init()
{
	Task = new BikePhysicalInput(this);
	Task->Run();
}

uint8 UBikeGameInstance::GetSpeed()
{
	return SpeedsArray[0];
}

void UBikeGameInstance::SetSpeed(uint8 value)
{
	SpeedsArray.Push(value);
	while (SpeedsArray.Num() > 1)
	{
		SpeedsArray.Pop();
	}
}
