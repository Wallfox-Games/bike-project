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

	SensorState = -1;
	MobileState = -1;

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
	StopPhysicalTask();
	StopMobileTask();
}

void UBikeGameInstance::SetPhysicalSpeed(float NewSpeed)
{
	if (SensorState == 0) SensorState = 1;
	PhysicalSpeed = NewSpeed * Circumference / 1000.f;
}

void UBikeGameInstance::SetMobileSpeed(float NewSpeed)
{
	MobileSpeed = NewSpeed;
}

float UBikeGameInstance::GetSpeed()
{
	if (SensorState) return PhysicalSpeed;
	else return MobileSpeed;
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

void UBikeGameInstance::StartPhysicalTask(bool LoadDevice)
{
	PhysicalTask = new BikePhysicalInput(this, LoadDevice);
}

void UBikeGameInstance::StopPhysicalTask()
{
	SensorState = false;
	if (PhysicalTask != nullptr) delete PhysicalTask;
	PhysicalTask = nullptr;
}

void UBikeGameInstance::StartMobileTask()
{
	MobileTask = new BikeMobileInput(this);
}

void UBikeGameInstance::StopMobileTask()
{
	MobileState = -1;
	MobileSpeed = -1.f;
	if (MobileTask != nullptr) delete MobileTask;
	MobileTask = nullptr;
}

int UBikeGameInstance::GetSensorState() const
{
	return SensorState;
}

void UBikeGameInstance::SetMobileState(int NewValue)
{
	MobileState = NewValue;
	MobileSpeed = -1.f;
}

int UBikeGameInstance::GetMobileState() const
{
	return MobileState;
}

void UBikeGameInstance::SetDeviceType(int NewValue)
{
	DeviceType = NewValue;
}

int UBikeGameInstance::GetDeviceType() const
{
	return DeviceType;
}

void UBikeGameInstance::SetDeviceAddress(FString NewValue)
{
	DeviceAddress = NewValue;
}

FString UBikeGameInstance::GetDeviceAddress() const
{
	return DeviceAddress;
}

void UBikeGameInstance::SetSensorState(bool NewValue)
{
	if (NewValue) SensorState = 0;
}