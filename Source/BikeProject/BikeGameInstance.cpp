// Fill out your copyright notice in the Description page of Project Settings.

#include "HAL/FileManager.h" 

#include "Engine/Engine.h" 



#include "BikeGameInstance.h"

void UBikeGameInstance::Init()
{
	Circumference = 2100;

	Task = new BikePhysicalInput(this);
}

void UBikeGameInstance::Shutdown()
{
	delete Task;
}

void UBikeGameInstance::FillArrays(short EventTime, short RevCount)
{
	EventTimes.Push(EventTime);
	while (EventTimes.Num() > 2)
	{
		EventTimes.RemoveAt(0);
	}

	RevolutionCounts.Push(RevCount);
	while (RevolutionCounts.Num() > 2)
	{
		RevolutionCounts.RemoveAt(0);
	}

	if (EventTimes.Num() == 2 && RevolutionCounts.Num() == 2)
	{
		float CircumferenceM = Circumference / 1000;
		float RevCountDelta = RevolutionCounts[1] - RevolutionCounts[0];
		float EventTimeDelta = EventTimes[1] - EventTimes[0];

		GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("RevCount: " + FString::SanitizeFloat(RevCountDelta)));
		GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("EventTime: " + FString::SanitizeFloat(EventTimeDelta)));

		currentSpeed = (CircumferenceM * RevCountDelta * 1024) / EventTimeDelta;
	}
}

int UBikeGameInstance::GetSpeed()
{
	return currentSpeed;
}

void UBikeGameInstance::SetCircumference(float newCircumference)
{
	Circumference = newCircumference;
}

