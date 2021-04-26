// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/SphereComponent.h" 

#include "BikeLaneActor.generated.h"

UCLASS()
class BIKEPROJECT_API ABikeLaneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABikeLaneActor();

	// Init Position Values
	void InitValues(float LaneWidth, float NewLaneSpeed);

protected:
	UPROPERTY(EditAnywhere)
		USphereComponent* CenterLane;
	UPROPERTY(EditAnywhere)
		USphereComponent* LeftLane;
	UPROPERTY(EditAnywhere)
		USphereComponent* RightLane;

	UPROPERTY()
		float LerpAlpha;
	UPROPERTY(EditAnywhere)
		float LaneSpeed;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(FVector Movement);
	void Rotate(float Rotation, FVector CentrePoint);

	FVector MoveLeft(bool NewMove, float DeltaTime);
	FVector MoveCenter(bool NewMove, float DeltaTime, FVector DinoPos);
	FVector MoveRight(bool NewMove, float DeltaTime);

	bool IsFinishedMove();
};
