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
	float MAXPOWER;
	UPROPERTY()
	float UPPERPOWER;
	UPROPERTY()
	float MIDDLEPOWER;
	UPROPERTY()
	float LOWERPOWER;

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
	UPROPERTY(BlueprintSetter=SetLaneBlocked)
	bool LaneBlocked;

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
	UStaticMeshComponent* PlayerVisibleComponent;

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
	void TurnActor(float Angle);

	UFUNCTION()
		void RotateLeft();
	UFUNCTION()
		void RotateRight();

	// Getter and Setter
	UFUNCTION(BlueprintCallable)
	float GetPowerLevel() const;
	UFUNCTION(BlueprintCallable)
	float GetRawPower(int Scale) const;

	UFUNCTION(BlueprintCallable)
	void LoadMaxPower();
	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float NewPower);

	UFUNCTION(BlueprintCallable)
	void SetLanePos(FVector Easy, FVector Med, FVector Hard);
	UFUNCTION(BlueprintCallable)
	void SetLaneBlocked(bool Blocking);
};
