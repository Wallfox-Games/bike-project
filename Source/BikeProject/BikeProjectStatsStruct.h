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
    UPROPERTY(BlueprintReadWrite)
    float TotalDistanceRan;
    UPROPERTY(BlueprintReadWrite)
    int TotalCurrency;
    UPROPERTY()
    int CostumeStatus[5];

    UPROPERTY(BlueprintReadWrite)
    int MainGameModeCount;
    UPROPERTY(BlueprintReadWrite)
    int EndlessGameModeCount;
    UPROPERTY(BlueprintReadWrite)
    int StagesComplete;
    UPROPERTY(BlueprintReadWrite)
    int BossChunks;
    
    FPlayerStats()
    {
        PlayerMaxPower[0] = 0;
        PlayerMaxPower[1] = 0;
        PlayerMaxPower[2] = 0;
        TotalDistanceRan = 0;
        TotalCurrency = 0;
        CostumeStatus[0] = 0;
        CostumeStatus[1] = 0;
        CostumeStatus[2] = 0;
        CostumeStatus[3] = 0;
        CostumeStatus[4] = 0;
        CostumeStatus[5] = 0;
        MainGameModeCount = 0;
        EndlessGameModeCount = 0;
        StagesComplete = 0;
        BossChunks = 0;
    }
};
