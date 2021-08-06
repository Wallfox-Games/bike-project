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
    float PlayerMaxPower[3];
    UPROPERTY()
    float TotalDistanceRan;
    UPROPERTY()
    int TotalCurrency;

    UPROPERTY()
    int MainGameModeCount;
    UPROPERTY()
    int EndlessGameModeCount;
    UPROPERTY()
    int StagesComplete;
    UPROPERTY()
    int BossChunks;

    FPlayerStats()
    {
        PlayerMaxPower[0] = 0;
        PlayerMaxPower[1] = 0;
        PlayerMaxPower[2] = 0;
        TotalDistanceRan = 0;
        TotalCurrency = 0;
        MainGameModeCount = 0;
        EndlessGameModeCount = 0;
        StagesComplete = 0;
        BossChunks = 0;
    }
};
