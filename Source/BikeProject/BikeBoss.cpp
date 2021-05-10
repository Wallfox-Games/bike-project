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

	SpeedBase = 400.f;
	SpeedMultiplier = 600.f;
	fleeing = false;
	fleeingCounter = 0;
}

// Called when the game starts or when spawned
void ABikeBoss::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABikeBoss::Movement(float playerLevel, float playerMax, bool paused)
{
	if (paused == false)
	{
		if (fleeing == true)
		{
			Velocity.X = (SpeedBase + FMath::Clamp(playerLevel / playerMax, 0.f, 1.f) * SpeedMultiplier)*2;
			if (fleeingCounter == 180)
			{
				fleeing = false;
				fleeingCounter = 0;
			}
			else
			{
				fleeingCounter++;
			}
		}
		else
		{
			Velocity.X = (SpeedBase + FMath::Clamp(playerLevel / playerMax, 0.f, 1.f) * SpeedMultiplier);
		}
	}
	else
	{
		Velocity.X = 0;
	}
}

void ABikeBoss::SpawnBullet(FVector PlayerPos)
{

}

void ABikeBoss::Death()
{
}

void ABikeBoss::SetFleeing(bool newFleeing)
{
	fleeing = newFleeing;
}

bool ABikeBoss::GetFleeing() const
{
	return fleeing;
}

// Called every frame
void ABikeBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

