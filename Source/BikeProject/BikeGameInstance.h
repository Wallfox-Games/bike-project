// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BikePhysicalInput.h"
#include "BikeGameInstance.generated.h"

UENUM(BlueprintType)
enum class Gamestate : uint8 {
	GSmenu = 0 UMETA(DisplayName = "Menu"),
	GStutorial = 1 UMETA(DisplayName = "Tutorial"),
	GSmain = 2 UMETA(DisplayName = "Main"),
};

UCLASS()
class BIKEPROJECT_API UBikeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void Init() override;
	virtual void Shutdown() override;

	void FillArrays(unsigned short EventTime, unsigned short RevCount);
	UFUNCTION()
	float GetSpeed();

	UFUNCTION()
	void SetCircumference(float newCircumference);

	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float newMaxPower);
	UFUNCTION(BlueprintCallable)
	float GetMaxPower() const;
	UFUNCTION(BlueprintCallable)
	bool GetTutorialState() const;
	UFUNCTION()
	void SetSensorState(bool NewValue);
	UFUNCTION(BlueprintCallable)
	bool GetSensorState() const;

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
	UFUNCTION(BlueprintCallable)
		void SetBossActive(bool newActive);
	UFUNCTION(BlueprintCallable)
		bool GetBossActive() const;
	UFUNCTION(BlueprintCallable)
		void SetBossDefeated(bool newDefeated);
	UFUNCTION(BlueprintCallable)
		bool GetBossDefeated() const;
	UFUNCTION(BlueprintCallable)
		void SetGameState(int newState);
	UFUNCTION(BlueprintCallable)
		int GetGameState() const;
	// getter and setter for if the player was hit by an obstacle 
private:
	TArray<unsigned short> EventTimes;
	TArray<unsigned short> RevolutionCounts;
	UPROPERTY()
	float Circumference;

	UPROPERTY()
	float currentSpeed;
	BikePhysicalInput* Task;

	UPROPERTY(BlueprintSetter = SetMaxPower, BlueprintGetter = GetMaxPower)
	float MAXPOWER;
	UPROPERTY(BlueprintGetter = GetTutorialState)
	bool TutorialState;
	UPROPERTY(BlueprintGetter = GetSensorState)
	bool SensorState;

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
	UPROPERTY(EditAnywhere, BlueprintSetter = SetBossActive, BlueprintGetter = GetBossActive)
		bool Active;
	UPROPERTY(EditAnywhere, BlueprintSetter = SetBossDefeated, BlueprintGetter = GetBossDefeated)
		bool BDefeated;
	Gamestate gameState;
	UPROPERTY(EditAnywhere, BlueprintSetter = SetGameState, BlueprintGetter = GetGameState)
		int gameStateInt;
};
