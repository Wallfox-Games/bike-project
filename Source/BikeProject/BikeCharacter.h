// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BikeMovementComponent.h"
#include "GameFramework/Pawn.h"
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
	UPROPERTY(BlueprintGetter=GetRawPower)
	float PowerLevel;
	UPROPERTY()
	float PowerLevelOld;
	UPROPERTY(EditAnywhere, BlueprintSetter = SetMaxPower)
	float MAXPOWER;
	UPROPERTY()
	float UPPERPOWER;
	UPROPERTY()
	float MIDDLEPOWER;
	UPROPERTY()
	float LOWERPOWER;
	UPROPERTY(BlueprintGetter = GetTutorialState)
	bool TutorialState;

	UPROPERTY(EditAnywhere)
	float LaneWidth;
	UPROPERTY(EditAnywhere)
	float LaneSpeed;
	UPROPERTY(EditAnywhere)
	float SpeedBase;
	UPROPERTY(EditAnywhere)
	float SpeedMultiplier;

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

	UPROPERTY()
		bool PedalLeft;
	UPROPERTY()
		bool PedalRight;

	UFUNCTION()
		void Movement(float Value);
	UFUNCTION()
		void MoveUp();
	UFUNCTION()
		void MoveDown();
	UFUNCTION()
		void PedalLeftStart();
	UFUNCTION()
		void PedalRightStart();
	UFUNCTION()
		void PedalLeftEnd();
	UFUNCTION()
		void PedalRightEnd();
	UFUNCTION()
		void PedalLeftAxis(float Value);
	UFUNCTION()
		void PedalRightAxis(float Value);

	UFUNCTION()
		void AddTime();
	UFUNCTION()
		void CalculateBPM();

	UFUNCTION()
		void MoveHard();
	UFUNCTION()
		void MoveMed();
	UFUNCTION()
		void MoveEasy();
	UFUNCTION()
		void MoveHardDT(float DeltaTime);
	UFUNCTION()
		void MoveMedDT(float DeltaTime);
	UFUNCTION()
		void MoveEasyDT(float DeltaTime);

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
	
	// Getter and Setter
	UFUNCTION(BlueprintCallable)
	bool GetTutorialState() const;
	UFUNCTION(BlueprintCallable)
	float GetPowerLevel() const;
	UFUNCTION(BlueprintCallable)
	float GetRawPower() const;
	UFUNCTION(BlueprintCallable)
	void SetMaxPower(float newMaxPower);
};
