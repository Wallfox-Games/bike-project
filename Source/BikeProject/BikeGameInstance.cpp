// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeGameInstance.h"

#include "BikeProject.h"
#include "HAL/FileManager.h" 
#include "Engine/Engine.h" 

void UBikeGameInstance::Init()
{
	Circumference = 2100;
	CurrentSpeed = -1.f;

	SensorState = false;

	Task = new BikePhysicalInput(this);

	// Only load game stats if the load .sav file exists
	const FString SaveSlotName = FString(TEXT("PlayerSaveSlot"));

	// Checks for save data, if it exists then it loads the maximum power in the save and sets tutorial state to false (player has already done tutorial)
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		class UBikeProjectSaveGame* LoadInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UBikeProjectSaveGame::StaticClass()));
		LoadInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance->SaveSlotName, LoadInstance->UserIndex));
		MAXPOWER = LoadInstance->PlayerMaxPower;
		TutorialState = false;
	}
	// Else set MAXPOWER to high value and tutorial state to true
	else
	{
		// In tutorial mode
		TutorialState = true;
	}
}

void UBikeGameInstance::Shutdown()
{
	delete Task;
}

void UBikeGameInstance::FillArrays(unsigned short EventTime, unsigned short RevCount)
{

	// Push values into arrays and trim array size to 3
	EventTimes.Push(EventTime);
	while (EventTimes.Num() > 3)
	{
		EventTimes.RemoveAt(0);
	}

	RevolutionCounts.Push(RevCount);
	while (RevolutionCounts.Num() > 3)
	{
		RevolutionCounts.RemoveAt(0);
	}

	// Check if array is filled before calculating speed (m/s)
	if (EventTimes.Num() == 3 && RevolutionCounts.Num() == 3)
	{
		// Convert Circumference to Meters
		float CircumferenceM = Circumference / 1000;

		// Find difference between event times, if rollover then add max value
		float EventTimeDelta = EventTimes[2] - EventTimes[0];
		if (EventTimes[2] < EventTimes[0]) EventTimeDelta += 65536;

		float RevCountDelta = RevolutionCounts[2] - RevolutionCounts[0];
		if (RevolutionCounts[2] < RevolutionCounts[0]) RevCountDelta += 65536;

		//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, TEXT("RevDelta: " + FString::SanitizeFloat(RevCountDelta)));
		//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, TEXT("EventDelta: " + FString::SanitizeFloat(EventTimeDelta)));

		// Set current speed to new value
		if (EventTimeDelta != 0 || RevCountDelta != 0) CurrentSpeed = ((float)CircumferenceM * RevCountDelta * 1024.f) / EventTimeDelta;
		else CurrentSpeed = 0;
	}
}

float UBikeGameInstance::GetSpeed() const
{
	return CurrentSpeed;
}

void UBikeGameInstance::SetCircumference(float newCircumference)
{
	Circumference = newCircumference;
}

void UBikeGameInstance::SetMaxPower(float newMaxPower)
{
	MAXPOWER = newMaxPower;

	if (UBikeProjectSaveGame* SaveGameInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UBikeProjectSaveGame::StaticClass())))
	{
		// Set data on the savegame object.
		SaveGameInstance->PlayerMaxPower = MAXPOWER;

		// Save the data immediately.
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex))
		{
			// Save succeeded.
			TutorialState = false;
		}
	}
}

float UBikeGameInstance::GetMaxPower() const
{
	return MAXPOWER;
}

bool UBikeGameInstance::GetTutorialState() const
{
	return TutorialState;
}

bool UBikeGameInstance::GetSensorState() const
{
	return SensorState;
}

void UBikeGameInstance::SetSensorState(bool NewValue)
{
	SensorState = NewValue;
}