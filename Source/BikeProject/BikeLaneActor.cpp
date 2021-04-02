// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeLaneActor.h"

#include "Engine/Engine.h" 

// Sets default values
ABikeLaneActor::ABikeLaneActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CenterLane = CreateDefaultSubobject<USphereComponent>(TEXT("CenterLaneComponent"));
	CenterLane->SetupAttachment(RootComponent);
	LeftLane = CreateDefaultSubobject<USphereComponent>(TEXT("LeftLaneComponent"));
	LeftLane->SetupAttachment(CenterLane);
	RightLane = CreateDefaultSubobject<USphereComponent>(TEXT("RightLaneComponent"));
	RightLane->SetupAttachment(CenterLane);

	CenterLane->SetGenerateOverlapEvents(false);
	LeftLane->SetGenerateOverlapEvents(false);
	RightLane->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void ABikeLaneActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABikeLaneActor::InitValues(float LaneWidth, float NewLaneSpeed)
{
	LeftLane->SetRelativeLocation(FVector(0.f, -LaneWidth, 0.f));
	RightLane->SetRelativeLocation(FVector(0.f, LaneWidth, 0.f));

	LaneSpeed = NewLaneSpeed;
}

// Called every frame
void ABikeLaneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABikeLaneActor::Move(FVector Movement)
{
	SetActorLocation(GetActorLocation() + Movement);
}

void ABikeLaneActor::Rotate(float Rotation)
{
	CenterLane->SetWorldRotation(FRotator(0, Rotation, 0));
}

FVector ABikeLaneActor::MoveLeft(bool NewMove, float DeltaTime)
{
	if (NewMove) LerpAlpha = 0;

	LerpAlpha += DeltaTime * LaneSpeed;
	LerpAlpha = FMath::Clamp(LerpAlpha, 0.f, 1.f);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Alpha: ") + FString::SanitizeFloat(LerpAlpha), true);

	FVector NewPos = FMath::Lerp(GetActorLocation(), GetActorLocation() + LeftLane->GetRelativeLocation(), LerpAlpha);

	return NewPos;
}

FVector ABikeLaneActor::MoveCenter(bool NewMove, float DeltaTime, FVector DinoPos)
{
	DinoPos.Z = GetActorLocation().Z;
	float LeftDistance = FVector(DinoPos - (GetActorLocation() + LeftLane->GetRelativeLocation())).Size();
	float RightDistance = FVector(DinoPos - (GetActorLocation() + RightLane->GetRelativeLocation())).Size();

	if (NewMove) LerpAlpha = 0;

	LerpAlpha += DeltaTime * LaneSpeed;
	LerpAlpha = FMath::Clamp(LerpAlpha, 0.f, 1.f);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Alpha: ") + FString::SanitizeFloat(LerpAlpha), true);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("DinoPos: ") + FString::SanitizeFloat(LeftDistance), true);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Actor: ") + FString::SanitizeFloat(RightDistance), true);

	FVector NewPos = GetActorLocation();
	if (LeftDistance < RightDistance) NewPos = FMath::Lerp(GetActorLocation() + LeftLane->GetRelativeLocation(), GetActorLocation(), LerpAlpha);
	else NewPos = FMath::Lerp(GetActorLocation() + RightLane->GetRelativeLocation(), GetActorLocation(), LerpAlpha);

	return NewPos;
}

FVector ABikeLaneActor::MoveRight(bool NewMove, float DeltaTime)
{
	if (NewMove) LerpAlpha = 0;

	LerpAlpha += DeltaTime * LaneSpeed;
	LerpAlpha = FMath::Clamp(LerpAlpha, 0.f, 1.f);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Alpha: ") + FString::SanitizeFloat(LerpAlpha), true);

	FVector NewPos = FMath::Lerp(GetActorLocation(), GetActorLocation() + RightLane->GetRelativeLocation(), LerpAlpha);

	return NewPos;
}

bool ABikeLaneActor::IsFinishedMove()
{
	if (LerpAlpha >= 1) return true;
	else return false;
}

