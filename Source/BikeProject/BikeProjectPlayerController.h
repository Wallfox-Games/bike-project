// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BikeProjectPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BIKEPROJECT_API ABikeProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	public:
		virtual void BeginPlay() override;
};
