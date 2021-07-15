// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "BikeProjectStatsStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPlayerStats
{
    GENERATED_BODY()

    // Use UPROPERTY() to decorate member variables as they allow for easier integration with network replication as well as potential garbage collection processing
    UPROPERTY()
    float PlayerMaxPower;

    UPROPERTY()
    int TotalCurrency;

    UPROPERTY()
    float TotalDistanceRan;

    UPROPERTY()
    int BossesDefeated;

    UPROPERTY()
    int MainGameModeCount;

    UPROPERTY()
    int StagesComplete;

    FPlayerStats()
    {
        PlayerMaxPower = 0;
        TotalCurrency = 0;
        TotalDistanceRan = 0;
        BossesDefeated = 0;
        MainGameModeCount = 0;
        StagesComplete = 0;
    }
};
