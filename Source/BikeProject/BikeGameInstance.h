// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BikePhysicalInput.h"
#include "BikeMobileInput.h"
#include "BikeProjectStatsStruct.h"
#include "BikeGameInstance.generated.h"

UCLASS()
class BIKEPROJECT_API UBikeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION()
	void SetPhysicalSpeed(float NewSpeed);
	UFUNCTION()
	void SetMobileSpeed(float NewSpeed);
	UFUNCTION(BlueprintCallable)
	float GetSpeed();
	UFUNCTION(BlueprintCallable)
	float GetPhysicalSpeed() const;
	UFUNCTION(BlueprintCallable)
	float GetMobileSpeed() const;

	UFUNCTION()
	void SetCircumference(float newCircumference);

	UFUNCTION(BlueprintCallable)
	FPlayerStats GetPlayerStats() const;
	UFUNCTION(BlueprintCallable)
	void SavePlayerStats();
	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float newMaxPower);
	UFUNCTION(BlueprintCallable)
	float GetMaxPower() const;
	UFUNCTION(BlueprintCallable)
	void IncDistTravelled(float Distance);
	UFUNCTION(BlueprintCallable)
	void IncBossChunks();
	UFUNCTION(BlueprintCallable)
	void IncMainGMCount();
	UFUNCTION(BlueprintCallable)
	void IncStagesComplete();

	UFUNCTION(BlueprintCallable)
	bool GetTutorialState() const;

	UFUNCTION(BlueprintCallable)
	void StartPhysicalTask(bool LoadDevice);
	UFUNCTION(BlueprintCallable)
	void StopPhysicalTask();
	UFUNCTION(BlueprintCallable)
	void StartMobileTask();
	UFUNCTION(BlueprintCallable)
	void StopMobileTask();

	UFUNCTION()
	void SetSensorState(bool NewValue);
	UFUNCTION(BlueprintCallable)
	bool GetSensorState() const;
	UFUNCTION(BlueprintCallable)
	void SetMobileState(int NewValue);
	UFUNCTION(BlueprintCallable)
	int GetMobileState() const;
	UFUNCTION(BlueprintCallable)
	void SetDeviceType(int NewValue);
	UFUNCTION(BlueprintCallable)
	int GetDeviceType() const;
	UFUNCTION()
	void SetDeviceAddress(FString NewValue);
	UFUNCTION(BlueprintCallable)
	FString GetDeviceAddress() const;

private:
	TArray<unsigned short> EventTimes;
	TArray<unsigned short> RevolutionCounts;
	UPROPERTY()
	float Circumference;

	BikePhysicalInput* PhysicalTask;
	BikeMobileInput* MobileTask;
	UPROPERTY(BlueprintGetter = GetPhysicalSpeed)
	float PhysicalSpeed;
	UPROPERTY(BlueprintGetter = GetDeviceType)
	int DeviceType;
	UPROPERTY(BlueprintGetter = GetMobileSpeed)
	float MobileSpeed;
	UPROPERTY(BlueprintGetter = GetDeviceAddress)
	FString DeviceAddress;

	UPROPERTY(BlueprintGetter = GetPlayerStats)
	FPlayerStats PlayerStats;
	UPROPERTY(BlueprintGetter = GetTutorialState)
	bool TutorialState;
	UPROPERTY(BlueprintGetter = GetSensorState)
	bool SensorState;
	UPROPERTY(BlueprintSetter = SetMobileState, BlueprintGetter = GetMobileState)
	int MobileState;
};
