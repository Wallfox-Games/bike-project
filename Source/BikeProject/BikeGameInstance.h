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
	virtual void Init() override;

	void FillArrays(short EventTime, short RevCount);
	int GetSpeed();

	void SetCircumference(float newCircumference);

private:
	TArray<short> EventTimes;
	TArray<short> RevolutionCounts;
	float Circumference;

	int currentSpeed;
	BikePhysicalInput* Task;
};
