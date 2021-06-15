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

	UFUNCTION()
	void StartPhysicalTask();
	UFUNCTION()
	void StartMobileTask();

	UFUNCTION()
	void SetSensorState(bool NewValue);
	UFUNCTION(BlueprintCallable)
	bool GetSensorState() const;
	UFUNCTION()
	void SetMobileState(bool NewValue);
	UFUNCTION(BlueprintCallable)
	bool GetMobileState() const;

private:
	TArray<unsigned short> EventTimes;
	TArray<unsigned short> RevolutionCounts;
	UPROPERTY()
	float Circumference;

	BikePhysicalInput* PhysicalTask;
	BikeMobileInput* MobileTask;
	UPROPERTY()
	float PhysicalSpeed;
	UPROPERTY()
	float MobileSpeed;

	UPROPERTY(BlueprintSetter = SetMaxPower, BlueprintGetter = GetMaxPower)
	float MAXPOWER;
	UPROPERTY(BlueprintGetter = GetTutorialState)
	bool TutorialState;
	UPROPERTY(BlueprintGetter = GetSensorState)
	bool SensorState;
	UPROPERTY(BlueprintGetter = GetMobileState)
	bool MobileState;
};
