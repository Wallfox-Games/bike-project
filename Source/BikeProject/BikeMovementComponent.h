// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "BikeMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BIKEPROJECT_API UBikeMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
