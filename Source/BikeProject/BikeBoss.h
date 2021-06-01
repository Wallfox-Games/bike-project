// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "BikeCharacter.h"

#include "Components/BoxComponent.h" 
#include "DestructibleComponent.h"

#include "BikeBoss.generated.h"

UENUM(BlueprintType)
enum EBossState{
	BSE_Moving			UMETA(DisplayName = "Moving"),
	BSE_Healthy			UMETA(DisplayName = "Healthy"),
	BSE_Vulnerable		UMETA(DisplayName = "Vulnerable"),
	BSE_Attacking		UMETA(DisplayName = "Attacking"),
	BSE_Defeated		UMETA(DisplayName = "Defeated"),
};

UCLASS()
class BIKEPROJECT_API ABikeBoss : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABikeBoss();

protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDestructibleComponent* BossDestructibleComponent;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* BossCameraSpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* BossCamera;

	UPROPERTY(EditAnywhere)
	FTransform HealthyCamTransform;
	UPROPERTY(EditAnywhere)
	float HealthyCameraDistance;
	UPROPERTY(EditAnywhere)
	float HealthyCameraFOV;
	UPROPERTY(EditAnywhere)
	FTransform VulnerableCamTransform;
	UPROPERTY(EditAnywhere)
	float VulnerableCameraDistance;
	UPROPERTY(EditAnywhere)
	float VulnerableCameraFOV;
	UPROPERTY()
	float CameraLerpAlpha;

	UPROPERTY()
	class UBikeMovementComponent* MovementComponent;

	UPROPERTY()
	int Health;
	UPROPERTY()
	TEnumAsByte<EBossState> BossStateEnum;
	UPROPERTY()
	float TargetSeconds;
	UPROPERTY()
	float TargetMultiplier;
	UPROPERTY()
	float TargetAttackPower;
	UPROPERTY()
	float CurrentTime;
	UPROPERTY()
	float CurrentAttackPower;
	UPROPERTY(EditAnywhere)
	bool CanHit;

	UPROPERTY()
	ABikeCharacter* PlayerPtr;
	UPROPERTY()
	ABikeLaneActor* BikeLanes;
	
	UPROPERTY()
	int CurrentLane;
	UPROPERTY()
	bool LaneChange;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void Movement();

	UFUNCTION()
		void SetCameraPosition(float DeltaTime);

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitValues(ABikeCharacter* NewPtr, int NewHealth, float NewSeconds, float NewMultiplier, float DeltaTime);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	float GetCurrentAttackPower() const;
	UFUNCTION(BlueprintCallable)
	float GetPercentageAttackPower() const;

	UFUNCTION(BlueprintCallable)
		float GetCurrentTime() const;
	UFUNCTION(BlueprintCallable)
		float GetPercentageTime() const;
	UFUNCTION(BlueprintCallable)
		float GetTimeToGo() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnHit();
	UFUNCTION(BlueprintCallable)
	int GetHealth() const;

	UFUNCTION(BlueprintCallable)
	EBossState GetBossEnum() const;
};
