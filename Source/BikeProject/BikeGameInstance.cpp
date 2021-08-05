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
	MobileState = -1;

	MobileTask = nullptr;
	PhysicalTask = nullptr;

	// Only load game stats if the load .sav file exists
	const FString SaveSlotName = FString(TEXT("PlayerSaveSlot"));

	// Checks for save data, if it exists then it loads the maximum power in the save and sets tutorial state to false (player has already done tutorial)
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		class UBikeProjectSaveGame* LoadInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UBikeProjectSaveGame::StaticClass()));
		LoadInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance->SaveSlotName, LoadInstance->UserIndex));
		PlayerStats = LoadInstance->SaveStats;
		TutorialState = false;
	}
	// Else set tutorial state to true
	else
	{
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
	switch (DeviceType)
	{
	case 1:
		PhysicalSpeed = NewSpeed * Circumference / 1000.f;
		break;
	case 2:
		PhysicalSpeed = NewSpeed * 60.f;
		break;
	case 3:
		PhysicalSpeed = NewSpeed;
		break;
	default:
		break;
	}
	
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

float UBikeGameInstance::GetPhysicalSpeed() const
{
	return PhysicalSpeed;
}

float UBikeGameInstance::GetMobileSpeed() const
{
	return MobileSpeed;
}

void UBikeGameInstance::SetCircumference(float newCircumference)
{
	Circumference = newCircumference;
}

FPlayerStats UBikeGameInstance::GetPlayerStats() const
{
	return PlayerStats;
}

void UBikeGameInstance::SavePlayerStats()
{
	if (UBikeProjectSaveGame* SaveGameInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UBikeProjectSaveGame::StaticClass())))
	{
		// Set data on the savegame object.
		SaveGameInstance->SaveStats = PlayerStats;

		// Save the data immediately.
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex))
		{
			// Save succeeded.
			TutorialState = false;
		}
	}
}

void UBikeGameInstance::SetMaxPower(float newMaxPower)
{
	PlayerStats.PlayerMaxPower = newMaxPower;	
}

float UBikeGameInstance::GetMaxPower() const
{
	return PlayerStats.PlayerMaxPower;
}

void UBikeGameInstance::IncDistTravelled(float Distance)
{
	PlayerStats.TotalDistanceRan += Distance;
}

void UBikeGameInstance::IncBossChunks()
{
	PlayerStats.BossChunks++;
}

void UBikeGameInstance::IncMainGMCount()
{
	PlayerStats.MainGameModeCount++;
}

void UBikeGameInstance::IncStagesComplete()
{
	PlayerStats.StagesComplete++;
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

bool UBikeGameInstance::GetSensorState() const
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

void UBikeGameInstance::SetMobileMessage(int NewValue)
{
	MobileMessage = NewValue;
}

int UBikeGameInstance::GetMobileMessage() const
{
	return MobileMessage;
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
	SensorState = NewValue;
}