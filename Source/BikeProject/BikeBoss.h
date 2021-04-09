// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BikeBoss.generated.h"


UCLASS()
class BIKEPROJECT_API ABikeBoss : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABikeBoss();

protected:
	UPROPERTY(EditAnywhere)
		USceneComponent* BossVisibleComponent;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
		void Movement();
	UFUNCTION(BlueprintCallable)
		void SpawnBullet(FVector PlayerPos);
	UFUNCTION(BlueprintCallable)
		void Death();

	UPROPERTY(EditAnywhere)
		FVector Velocity;
	UPROPERTY(EditAnywhere)
		float MAXPOWER;
	UPROPERTY(EditAnywhere)
		float PowerLevel;
	UPROPERTY(EditAnywhere)
		float SpeedBase;
	UPROPERTY(EditAnywhere)
		float SpeedMultiplier;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
