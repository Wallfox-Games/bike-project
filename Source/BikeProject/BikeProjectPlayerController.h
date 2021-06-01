// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "BikeProjectPlayerController.generated.h"

UENUM(BlueprintType)
enum EPlayerMove {
	PME_Normal			UMETA(DisplayName = "Normal Movement"),
	PME_LaneBoss		UMETA(DisplayName = "1 Lane Boss"),
	PME_MoveBoss		UMETA(DisplayName = "3 Lane Boss"),
	PME_AttackBoss		UMETA(DisplayName = "Attacking Boss"),
	PME_SlowDown		UMETA(DisplayName = "Slowing to Stop")
};

/**
 * 
 */
UCLASS()
class BIKEPROJECT_API ABikeProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	private:
	// Power Variables
	UPROPERTY(BlueprintGetter = GetPowerLevel)
	float PowerLevel;
	UPROPERTY()
	float PowerLevelKB;
	UPROPERTY()
	float PowerLevelBP;
	UPROPERTY()
	float PowerLevelCurrent;
	UPROPERTY()
	float PowerLevelTarget;
	UPROPERTY()
	float PowerAlpha;
	UPROPERTY()
	float PowerLevelAuto;

	// Movement Checks
	UPROPERTY(BlueprintGetter = GetMovePauseBlocked, BlueprintSetter = SetMovePauseBlocked)
		bool MovePauseBlocked;
	UPROPERTY(BlueprintGetter = GetMoveUIBlocked, BlueprintSetter = SetMoveUIBlocked)
		bool MoveUIBlocked;

	// Manual Pedalling
	UPROPERTY()
		TArray<double> PedalTimes;
	UPROPERTY()
		int ARRAYMAXSIZE = 10;
	UPROPERTY()
		double RPM;

	UPROPERTY()
		double TimeStartLeft;
	UPROPERTY()
		double TimeStartRight;

	UFUNCTION()
		void PedalLeftStart();
	UFUNCTION()
		void PedalRightStart();

	UFUNCTION()
		void AddTime();
	UFUNCTION()
		void CalculateBPM();

	UFUNCTION()
		void PowerTransition(float DeltaTime, float NewPower);

	// Gameplay Variables
	UPROPERTY()
		int PlayerHealth;
	UPROPERTY()
		int ComboMeter;
	UPROPERTY()
		TEnumAsByte<EPlayerMove> PlayerMoveEnum;

	public:
	ABikeProjectPlayerController();

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	float GetPowerLevel() const;

	UFUNCTION(BlueprintCallable)
	bool GetMovePauseBlocked() const;
	UFUNCTION(BlueprintCallable)
	void SetMovePauseBlocked(bool Blocking);
	UFUNCTION(BlueprintCallable)
	bool GetMoveUIBlocked() const;
	UFUNCTION(BlueprintCallable)
	void SetMoveUIBlocked(bool Blocking);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void IncPlayerHealth(bool MaxHealth);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void PlayerHit(bool OffScreen);
	UFUNCTION(BlueprintCallable)
		int GetPlayerHealth() const;

	UFUNCTION(BlueprintCallable)
		void SetComboMeter();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ChangeComboMeter(bool PositiveChange);
	UFUNCTION(BlueprintCallable)
		int GetComboMeter() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SetMoveEnum(EPlayerMove NewState, float DeltaTime);
	UFUNCTION(BlueprintCallable)
		EPlayerMove GetMoveEnum() const;
};
