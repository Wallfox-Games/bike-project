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

	uint8 GetSpeed();
	void SetSpeed(uint8 newSpeed);

private:
	TArray<uint8> SpeedsArray;
	BikePhysicalInput* Task;
};
