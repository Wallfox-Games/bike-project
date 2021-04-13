// Fill out your copyright notice in the Description page of Project Settings.


#include "BikeBoss.h"

// Sets default values
ABikeBoss::ABikeBoss()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BossVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisableComponent"));

	BossVisibleComponent->SetupAttachment(RootComponent);

	SpeedBase = 200.f;
	SpeedMultiplier = 300.f;
}

// Called when the game starts or when spawned
void ABikeBoss::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABikeBoss::Movement()
{
	Velocity.X = SpeedBase + 0.8f * SpeedMultiplier;
}

void ABikeBoss::SpawnBullet(FVector PlayerPos)
{

}

void ABikeBoss::Death()
{
}

// Called every frame
void ABikeBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Movement();
	if (!Velocity.IsZero())
	{
		FVector NewPos = GetActorLocation() + (Velocity * DeltaTime);
		SetActorLocation(NewPos);
	}

}

// Called to bind functionality to input
void ABikeBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

