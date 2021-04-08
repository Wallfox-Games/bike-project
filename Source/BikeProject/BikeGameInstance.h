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
	UFUNCTION(BlueprintCallable)
		void SetCombo(int newCombo);
	UFUNCTION(BlueprintCallable)
		int GetCombo() const;
	//getter and setter for combo total
	UFUNCTION(BlueprintCallable)
		void SetMultiplier(int newMultiplier);
	UFUNCTION(BlueprintCallable)
		int GetMultiplier() const;
	// getter and setter for combo multiplier
	UFUNCTION(BlueprintCallable)
		void SetHit(bool newhit);
	UFUNCTION(BlueprintCallable)
		bool GetHit() const;
	// getter and setter for if the player was hit by an obstacle 
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
	UPROPERTY(EditAnywhere, BlueprintSetter = SetCombo, BlueprintGetter = GetCombo)
		int Combo;
	UPROPERTY(EditAnywhere, BlueprintSetter = SetMultiplier, BlueprintGetter = GetMultiplier)
		int Multiplier;
	UPROPERTY(EditAnywhere, BlueprintSetter = SetHit, BlueprintGetter = GetHit)
		bool hit;
};
