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

	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float newMaxPower);
	UFUNCTION(BlueprintCallable)
	float GetMaxPower() const;
	UFUNCTION(BlueprintCallable)
	bool GetTutorialState() const;


	UFUNCTION(BlueprintCallable)
	void SetPlayerHealth(int newHealth);
	UFUNCTION(BlueprintCallable)
	int GetPlayerHealth() const;
	//getter and setter for players health bar
	UFUNCTION(BlueprintCallable)
	void SetBossHealth(int newHealth);
	UFUNCTION(BlueprintCallable)
	int GetBossHealth() const;
	//getter and setter for boss health bar

private:
	TArray<short> EventTimes;
	TArray<short> RevolutionCounts;
	UPROPERTY()
	float Circumference;

	UPROPERTY()
	int currentSpeed;
	BikePhysicalInput* Task;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetMaxPower, BlueprintGetter = GetMaxPower)
	float MAXPOWER;
	UPROPERTY(BlueprintGetter = GetTutorialState)
	bool TutorialState;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetPlayerHealth, BlueprintGetter = GetPlayerHealth)
	int PlayerHealth;
	UPROPERTY(EditAnywhere, BlueprintSetter = SetPlayerHealth, BlueprintGetter = GetPlayerHealth)
    int BossHealth;
};
