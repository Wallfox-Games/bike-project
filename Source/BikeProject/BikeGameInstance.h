// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BikePhysicalInput.h"
#include "BikeGameInstance.generated.h"

UCLASS()
class BIKEPROJECT_API UBikeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void Init() override;
	virtual void Shutdown() override;

	void FillArrays(unsigned short EventTime, unsigned short RevCount);
	UFUNCTION(BlueprintCallable)
	float GetSpeed() const;

	UFUNCTION()
	void SetCircumference(float newCircumference);

	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float newMaxPower);
	UFUNCTION(BlueprintCallable)
	float GetMaxPower() const;
	UFUNCTION(BlueprintCallable)
	bool GetTutorialState() const;
	UFUNCTION()
	void SetSensorState(bool NewValue);
	UFUNCTION(BlueprintCallable)
	bool GetSensorState() const;

private:
	TArray<unsigned short> EventTimes;
	TArray<unsigned short> RevolutionCounts;
	UPROPERTY()
	float Circumference;

	BikePhysicalInput* Task;
	UPROPERTY(BlueprintGetter = GetSpeed)
	float CurrentSpeed;

	UPROPERTY(BlueprintSetter = SetMaxPower, BlueprintGetter = GetMaxPower)
	float MAXPOWER;
	UPROPERTY(BlueprintGetter = GetTutorialState)
	bool TutorialState;
	UPROPERTY(BlueprintGetter = GetSensorState)
	bool SensorState;
};
