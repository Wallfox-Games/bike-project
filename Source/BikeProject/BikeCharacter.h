// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
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
	UPROPERTY(BlueprintGetter=GetPowerLevel)
	float PowerLevel;
	UPROPERTY(BlueprintReadWrite)
	int MAXPOWER = 120;

	UPROPERTY(EditAnywhere)
	USceneComponent* PlayerVisibleComponent;

	USpringArmComponent* PlayerCameraSpringArm;
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

	// Getter
	UFUNCTION(BlueprintCallable)
	float GetPowerLevel() const;
};
