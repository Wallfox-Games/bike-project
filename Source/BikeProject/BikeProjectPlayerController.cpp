// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeProjectPlayerController.h"

#include "BikeCharacter.h"
#include "BikeGameInstance.h"

ABikeProjectPlayerController::ABikeProjectPlayerController()
{
	// Sets the starting values for non bike input to current time, so the first input isn't very long
	TimeStartLeft = FPlatformTime::Seconds();
	TimeStartRight = FPlatformTime::Seconds();
	// Reserves memory for the pedal array
	PedalTimes.Reserve(ARRAYMAXSIZE + 1);

	// Default variables
	PowerLevelCurrent = 0;
	PowerLevelTarget = 0;
	PowerAlpha = 0;

	MovePauseBlocked = false;
	MoveUIBlocked = false;
}

void ABikeProjectPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameOnly());
}

// Called every frame
void ABikeProjectPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TempPower;

	UBikeGameInstance* GameInstanceRef = Cast<UBikeGameInstance>(GetGameInstance());
	PowerLevelBP = GameInstanceRef->GetSpeed();

	if (PowerLevelKB > PowerLevelBP)
	{
		PowerLevelKB -= 1.f * DeltaTime;
		TempPower = PowerLevelKB;
	}
	else TempPower = PowerLevelBP;

	PowerTransition(DeltaTime, TempPower);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Bike Input Speed: ") + FString::SanitizeFloat(PowerLevelBP), true);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Keyboard Input Speed: ") + FString::SanitizeFloat(PowerLevelKB), true);

	ABikeCharacter* PawnInstanceRef = Cast<ABikeCharacter>(GetPawn());
	PawnInstanceRef->SetCurrentPower(PowerLevel);

	if (!MovePauseBlocked && !MoveUIBlocked) PawnInstanceRef->Movement(DeltaTime);
}

// Called to bind functionality to input
void ABikeProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up "movement" bindings.
	InputComponent->BindAction("PedalLeftButton", IE_Pressed, this, &ABikeProjectPlayerController::PedalLeftStart);
	InputComponent->BindAction("PedalRightButton", IE_Pressed, this, &ABikeProjectPlayerController::PedalRightStart);
}

float ABikeProjectPlayerController::GetPowerLevel() const
{
	return PowerLevel;
}

// Sets time for left input and calls AddTime
void ABikeProjectPlayerController::PedalLeftStart()
{
	TimeStartLeft = FPlatformTime::Seconds();
	AddTime();
}

// Sets time for right input and calls AddTime
void ABikeProjectPlayerController::PedalRightStart()
{
	TimeStartRight = FPlatformTime::Seconds();
	AddTime();
}

// Finds the absolute difference between the two inputs and adds to the PedalTimes array
// Calls CalculateBPM
void ABikeProjectPlayerController::AddTime()
{
	double TimeValue = abs(TimeStartLeft - TimeStartRight);

	// Adds the value to the array and checks to make sure the array is ARRAYMAXSIZE long
	PedalTimes.Add(TimeValue);
	if (PedalTimes.Num() > ARRAYMAXSIZE) PedalTimes.RemoveAt(0);

	CalculateBPM();
}

// Calculates the PowerLevel
void ABikeProjectPlayerController::CalculateBPM()
{
	RPM = 0;
	for (auto& Time : PedalTimes)
	{
		RPM += Time;
	}
	// Length of one beat
	RPM = RPM / PedalTimes.Num();
	// Beats-per-second
	RPM = 1 / RPM;
	// Beats-per-minute
	RPM *= 60;

	// Set to power / RPM (roughly half)
	PowerLevelKB = RPM / 2;
}

void ABikeProjectPlayerController::PowerTransition(float DeltaTime, float NewPower)
{
	if (PowerLevelTarget != NewPower)
	{
		PowerLevelCurrent = PowerLevelTarget;
		PowerLevelTarget = NewPower;
		PowerAlpha = 0;
	}

	PowerAlpha += DeltaTime / 0.25f;
	PowerAlpha = FMath::Clamp(PowerAlpha, 0.f, 1.f);
	PowerLevel = FMath::Lerp(PowerLevelCurrent, PowerLevelTarget, PowerAlpha);
}

bool ABikeProjectPlayerController::GetMovePauseBlocked() const
{
	return MovePauseBlocked;
}

void ABikeProjectPlayerController::SetMovePauseBlocked(bool Blocking)
{
	MovePauseBlocked = Blocking;
}

bool ABikeProjectPlayerController::GetMoveUIBlocked() const
{
	return MoveUIBlocked;
}

void ABikeProjectPlayerController::SetMoveUIBlocked(bool Blocking)
{
	MoveUIBlocked = Blocking;
}
