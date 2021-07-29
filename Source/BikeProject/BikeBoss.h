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
	BSE_Moving			UMETA(DisplayName = "Moving into position"),
	BSE_Cooldown		UMETA(DisplayName = "Cooldown"),
	BSE_Attacking		UMETA(DisplayName = "Attacking player"),
	BSE_Despawning		UMETA(DisplayName = "Despawning mines"),
	BSE_Reloading		UMETA(DisplayName = "Reloading mines"),
	BSE_Vulnerable		UMETA(DisplayName = "Vulnerable to player"),
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
	USkeletalMeshComponent* BossVisibleComponent;
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

	UPROPERTY(EditAnywhere)
	FString ObstacleString;
	UPROPERTY()
	FString ObstacleStringTemp;
	UPROPERTY()
	int ObstacleCurrent;
	UPROPERTY()
	float ObstacleTick;
	UPROPERTY(BlueprintSetter = SetObstaclesDestroyed)
	bool ObstaclesDestroyed;
	UPROPERTY(EditAnywhere)
	float ObstacleMaxTick = 1.f;

	UPROPERTY()
	float CurrentTime;
	UPROPERTY()
	float CurrentAttackPower;

	UPROPERTY(EditAnywhere)
	bool CanHit;

	UPROPERTY(BlueprintGetter = GetCooldown)
	float Cooldown;
	UPROPERTY(EditAnywhere)
	float MaxCooldown = 10.f;

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
		void SetCameraPosition(float DeltaTime, ABikeProjectPlayerController* PlayerControllerPtr);

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitValues(ABikeCharacter* NewPtr, int NewHealth, float NewSeconds, float NewMultiplier, float DeltaTime);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeState(EBossState NewState);

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

	UFUNCTION(BlueprintCallable)
	float GetCooldown() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnMine();
	UFUNCTION(BlueprintCallable)
	void SetObstaclesDestroyed(bool IsDestroyed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnHit();
	UFUNCTION(BlueprintCallable)
	int GetHealth() const;

	UFUNCTION(BlueprintCallable)
	EBossState GetBossEnum() const;
};
