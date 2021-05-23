// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "BikeMovementComponent.h"
#include "BikeLaneActor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"

#include "BikeCharacter.generated.h"

UCLASS()
class BIKEPROJECT_API ABikeCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABikeCharacter();

protected:
	// Variables
	UPROPERTY()
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
	float MAXPOWER;
	UPROPERTY()
	float UPPERPOWER;
	UPROPERTY()
	float MIDDLEPOWER;
	UPROPERTY()
	float LOWERPOWER;
	UPROPERTY(EditAnywhere)
	float UpperPercent;
	UPROPERTY(EditAnywhere)
	float MiddlePercent;
	UPROPERTY(EditAnywhere)
	float LowerPercent;

	UPROPERTY(EditAnywhere)
	float CameraDistance;
	UPROPERTY(EditAnywhere)
	float FOVBase;
	UPROPERTY(EditAnywhere)
	float FOVMultiplier;
	UPROPERTY(EditAnywhere)
	float PPMed;
	UPROPERTY(EditAnywhere)
	float PPAlphaMult;
	UPROPERTY(BlueprintGetter = GetPostProcessAlpha)
	float PPAlpha;

	UPROPERTY(EditAnywhere)
	float LaneWidth;
	UPROPERTY(EditAnywhere)
	float LaneSpeed;
	UPROPERTY(EditAnywhere)
	float SpeedBase;
	UPROPERTY(EditAnywhere)
	float SpeedMultiplier;
	UPROPERTY()
	bool LaneSwitching;
	UPROPERTY()
	bool LaneBlocked;
	UPROPERTY(BlueprintGetter = GetMovePauseBlocked, BlueprintSetter = SetMovePauseBlocked)
	bool MovePauseBlocked;
	UPROPERTY(BlueprintGetter = GetMoveUIBlocked, BlueprintSetter = SetMoveUIBlocked)
	bool MoveUIBlocked;

	UPROPERTY()
	ABikeLaneActor* BikeLanes;
	UPROPERTY()
	FVector EasyLanePos;
	UPROPERTY()
	FVector MedLanePos;
	UPROPERTY()
	FVector HardLanePos;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* PlayerVisibleComponent;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* PlayerCameraSpringArm;
	UPROPERTY(EditAnywhere)
	UCameraComponent* PlayerCamera;


	// Handles input for moving forward.
	UPROPERTY()
		int PowerLane;
	UPROPERTY()
		TArray<double> PedalTimes;
	UPROPERTY(BlueprintReadWrite)
		int ARRAYMAXSIZE = 10;
	UPROPERTY()
		double RPM;

	UPROPERTY()
		double TimeStartLeft;
	UPROPERTY()
		double TimeStartRight;

	UPROPERTY()
		bool Attacking;

	UFUNCTION()
		void Movement(float Value);
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
	UFUNCTION()
	void PostProcessTransition(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void MoveNewLane(float DeltaTime);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Third-person camera.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* TPCameraComponent;

	// Movement component.
	UPROPERTY()
	class UBikeMovementComponent* MovementComponent;

	virtual UBikeMovementComponent* GetMovementComponent() const override;

	UFUNCTION(BlueprintCallable)
	void Turn(float Angle, FVector CenterPoint);

	// Getter and Setter
	UFUNCTION(BlueprintCallable)
	float GetPostProcessAlpha() const;
	UFUNCTION(BlueprintCallable)
	float GetPowerPercent() const;
	UFUNCTION(BlueprintCallable)
	float GetRawPower(int Scale) const;

	UFUNCTION(BlueprintCallable)
	void LoadMaxPower();
	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float NewPower);

	UFUNCTION(BlueprintCallable)
	void SetLanePos(FVector Easy, FVector Med, FVector Hard);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetLaneBlocked(bool Blocking);

	UFUNCTION(BlueprintCallable)
	bool GetMovePauseBlocked() const;
	UFUNCTION(BlueprintCallable)
	void SetMovePauseBlocked(bool Blocking);
	UFUNCTION(BlueprintCallable)
	bool GetMoveUIBlocked() const;
	UFUNCTION(BlueprintCallable)
	void SetMoveUIBlocked(bool Blocking);

	UFUNCTION(BlueprintCallable)
	void SetPowerLane(int newlane);
	UFUNCTION(BlueprintCallable)
	int GetPowerLane() const;
	UFUNCTION(BlueprintCallable)
	void SetAttacking(bool newattacking);
};
