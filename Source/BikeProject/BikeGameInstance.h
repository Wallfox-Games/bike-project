// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BikePhysicalInput.h"
#include "BikeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BIKEPROJECT_API UBikeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void Init() override;
	virtual void Shutdown() override;

	void FillArrays(short EventTime, short RevCount);
	UFUNCTION()
	int GetSpeed();

	UFUNCTION()
	void SetCircumference(float newCircumference);

private:
	TArray<short> EventTimes;
	TArray<short> RevolutionCounts;
	UPROPERTY()
	float Circumference;

	UPROPERTY()
	int currentSpeed;
	BikePhysicalInput* Task;
};
