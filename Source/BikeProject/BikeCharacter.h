// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "BikeCharacter.generated.h"

UCLASS()
class BIKEPROJECT_API ABikeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABikeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Variables
	float powerLevel;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles input for moving forward.
	UFUNCTION()
	void Movement(float Value);

	// Handles input for power display.
	UFUNCTION()
	void PowerDisplay(float Value);

	// Third-person camera.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* TPCameraComponent;
};
