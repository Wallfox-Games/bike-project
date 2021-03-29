// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeGameInstance.h"

#include "BikeProject.h"
#include "HAL/FileManager.h" 
#include "Engine/Engine.h" 

void UBikeGameInstance::Init()
{
	Circumference = 2100;

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
		MAXPOWER = 500;
		TutorialState = true;
	}
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

