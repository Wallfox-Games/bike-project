// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeGameInstance.h"

#include "BikeProject.h"
#include "HAL/FileManager.h" 
#include "Engine/Engine.h" 

void UBikeGameInstance::Init()
{
	Circumference = 2100;
	PhysicalSpeed = -1.f;
	MobileSpeed = -1.f;

	SensorState = false;
	MobileState = false;

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
	delete PhysicalTask;
	delete MobileTask;
}

void UBikeGameInstance::SetPhysicalSpeed(float NewSpeed)
{
	PhysicalSpeed = NewSpeed * Circumference / 1000.f;
}

void UBikeGameInstance::SetMobileSpeed(float NewSpeed)
{
	MobileSpeed = NewSpeed;
}

float UBikeGameInstance::GetSpeed()
{
	if (SensorState) return PhysicalSpeed;
	else if (MobileState) return MobileSpeed;
	else return -1.f;
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

void UBikeGameInstance::StartPhysicalTask()
{
	PhysicalTask = new BikePhysicalInput(this);
}

void UBikeGameInstance::StartMobileTask()
{
	MobileTask = new BikeMobileInput(this);
}

bool UBikeGameInstance::GetSensorState() const
{
	return SensorState;
}

void UBikeGameInstance::SetMobileState(bool NewValue)
{
	MobileState = NewValue;
}

bool UBikeGameInstance::GetMobileState() const
{
	return MobileState;
}

void UBikeGameInstance::SetSensorState(bool NewValue)
{
	SensorState = NewValue;
}