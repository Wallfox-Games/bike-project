// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BikePhysicalInput.h"
#include "BikeMobileInput.h"
#include "BikeGameInstance.generated.h"

UCLASS()
class BIKEPROJECT_API UBikeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void Init() override;
	virtual void Shutdown() override;

	void SetPhysicalSpeed(float NewSpeed);
	void SetMobileSpeed(float NewSpeed);
	UFUNCTION(BlueprintCallable)
	float GetSpeed();

	UFUNCTION()
	void SetCircumference(float newCircumference);

	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float newMaxPower);
	UFUNCTION(BlueprintCallable)
	float GetMaxPower() const;

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
	int GetSensorState() const;
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
	UPROPERTY()
	float PhysicalSpeed;
	UPROPERTY(BlueprintGetter = GetDeviceType)
	int DeviceType;
	UPROPERTY()
	float MobileSpeed;
	UPROPERTY(BlueprintGetter = GetDeviceAddress)
	FString DeviceAddress;

	UPROPERTY(BlueprintSetter = SetMaxPower, BlueprintGetter = GetMaxPower)
	float MAXPOWER;
	UPROPERTY(BlueprintGetter = GetTutorialState)
	bool TutorialState;
	UPROPERTY(BlueprintGetter = GetSensorState)
	int SensorState;
	UPROPERTY(BlueprintSetter = SetMobileState, BlueprintGetter = GetMobileState)
	int MobileState;
};
