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
		TutorialState = true;
	}
}

void UBikeGameInstance::Shutdown()
{
	delete Task;
}

void UBikeGameInstance::FillArrays(unsigned short EventTime, unsigned short RevCount)
{

	// Push values into arrays and trim array size to 2
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

	// Check if arrays are of size two and unique
	if (EventTimes.Num() == 2 && RevolutionCounts.Num() == 2)
	{
		if (EventTimes[1] != EventTimes[0] && RevolutionCounts[1] != RevolutionCounts[0])
		{
			// Convert Circumference to Meters
			float CircumferenceM = Circumference / 1000;

			// Find difference between event times, if rollover then add max value
			float EventTimeDelta = EventTimes[1] - EventTimes[0];
			if (EventTimes[1] < EventTimes[0]) EventTimeDelta += 65536;

			float RevCountDelta = RevolutionCounts[1] - RevolutionCounts[0];
			if (RevolutionCounts[1] < RevolutionCounts[0]) RevCountDelta += 65536;

			//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, TEXT("RevDelta: " + FString::SanitizeFloat(RevCountDelta)));
			//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, TEXT("EventDelta: " + FString::SanitizeFloat(EventTimeDelta)));

			// Set current speed to new value
			currentSpeed = ((float)CircumferenceM * RevCountDelta * 1024.f) / EventTimeDelta;
		}
		else
		{
			// If not unique then bike must be stationary
			currentSpeed = 0;
		}
	}
}

float UBikeGameInstance::GetSpeed()
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

void UBikeGameInstance::SetPlayerHealth(int newHealth)
{
	PlayerHealth = newHealth;
}

int UBikeGameInstance::GetPlayerHealth() const
{
	return PlayerHealth;
}

void UBikeGameInstance::SetBossHealth(int newHealth)
{
	BossHealth = newHealth;
}

int UBikeGameInstance::GetBossHealth() const
{
	return BossHealth;
}

void UBikeGameInstance::SetCombo(int newCombo)
{
	Combo = newCombo;
}

int UBikeGameInstance::GetCombo() const
{
	return Combo;
}

void UBikeGameInstance::SetMultiplier(int newMultiplier)
{
	Multiplier = newMultiplier;
}

int UBikeGameInstance::GetMultiplier() const
{
	return Multiplier;
}

void UBikeGameInstance::SetHit(bool newhit)
{
	hit = newhit;
}

bool UBikeGameInstance::GetHit() const
{
	return hit;
}

void UBikeGameInstance::SetBossActive(bool newActive)
{
	Active = newActive;
}

bool UBikeGameInstance::GetBossActive() const
{
	return Active;
}

void UBikeGameInstance::SetBossDefeated(bool newDefeated)
{
	BDefeated = newDefeated;
}

bool UBikeGameInstance::GetBossDefeated() const
{
	return BDefeated;
}

