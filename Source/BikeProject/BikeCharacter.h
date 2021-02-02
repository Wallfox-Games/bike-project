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
	float powerLevel;

	UPROPERTY(EditAnywhere)
	USceneComponent* PlayerVisibleComponent;

	USpringArmComponent* PlayerCameraSpringArm;
	UCameraComponent* PlayerCamera;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles input for moving forward.
	UFUNCTION()
	void Movement(float Value);

	// Third-person camera.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* TPCameraComponent;

	// Getter
	UFUNCTION(BlueprintCallable)
	float GetPowerLevel() const;
};
