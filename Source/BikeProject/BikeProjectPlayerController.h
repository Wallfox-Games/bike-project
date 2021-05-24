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

	UPROPERTY(BlueprintGetter = GetMovePauseBlocked, BlueprintSetter = SetMovePauseBlocked)
		bool MovePauseBlocked;
	UPROPERTY(BlueprintGetter = GetMoveUIBlocked, BlueprintSetter = SetMoveUIBlocked)
		bool MoveUIBlocked;

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
};
