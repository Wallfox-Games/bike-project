// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeCharacter.h"

#include "BikeGameInstance.h"

// Sets default values
ABikeCharacter::ABikeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create our components
	
	// Our root component will be a sphere that reacts to physics
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCapsuleRadius(25);
	CapsuleComponent->SetCapsuleHalfHeight(60);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));

	PlayerVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerVisibleComponent"));
	PlayerVisibleComponent->SetupAttachment(RootComponent);

	PlayerCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	PlayerCameraSpringArm->SetupAttachment(RootComponent);

	PlayerCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	PlayerCameraSpringArm->TargetArmLength = 400.f;
	PlayerCameraSpringArm->bEnableCameraLag = true;
	PlayerCameraSpringArm->CameraLagSpeed = 3.0f;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	PlayerCamera->SetupAttachment(PlayerCameraSpringArm, USpringArmComponent::SocketName);

	// Create an instance of our movement component, and tell it to update the root.
	MovementComponent = CreateDefaultSubobject<UBikeMovementComponent>(TEXT("CustomMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// Sets the starting values for non bike input to current time, so the first input isn't very long
	TimeStartLeft = FPlatformTime::Seconds();
	TimeStartRight = FPlatformTime::Seconds();
	// Reserves memory for the pedal array
	PedalTimes.Reserve(ARRAYMAXSIZE + 1);

	// Default variables for lanes and width (editable in blueprints)
	PowerLane = 1;
	PowerLevelCurrent = 0;
	PowerLevelTarget = 0;
	PowerAlpha = 0;
	LaneWidth = 90.f;
	LaneSpeed = 1.5f;
	SpeedBase = 200.f;
	SpeedMultiplier = 300.f;
	LaneBlocked = false;
	LaneSwitching = false;

}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	LoadMaxPower();

	FVector LanesLocation = GetActorLocation();
	FRotator LanesRotation(0.f);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoFail = true;
	SpawnInfo.Owner = this;
	BikeLanes = GetWorld()->SpawnActor<ABikeLaneActor>(LanesLocation, LanesRotation, SpawnInfo);

	BikeLanes->InitValues(LaneWidth, LaneSpeed);

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Biker is being used."));
}

// Called every frame
void ABikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TempPower;

	UBikeGameInstance* GameInstanceRef = Cast<UBikeGameInstance>(GetGameInstance());
	PowerLevelBP = GameInstanceRef->GetSpeed();

	if (PowerLevelKB > 0)
	{
		PowerLevelKB -= 1.f * DeltaTime;
		TempPower = PowerLevelKB;
	}
	else TempPower = PowerLevelBP;

	PowerTransition(DeltaTime, TempPower);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Bike Input Speed: ") + FString::SanitizeFloat(PowerLevelBP), true);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Keyboard Input Speed: ") + FString::SanitizeFloat(PowerLevelKB), true);

	// Determine current Lane
	// Snap to Upper and Lower Lanes
	if (!TutBlocked) Movement(DeltaTime);
}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAction("PedalLeftButton", IE_Pressed, this, &ABikeCharacter::PedalLeftStart);
	PlayerInputComponent->BindAction("PedalRightButton", IE_Pressed, this, &ABikeCharacter::PedalRightStart);
}

// Checks current power of dinosaur and moves them into correct lane
// Moves dinosaur forward at a speed value relative to power
void ABikeCharacter::Movement(float DeltaTime)
{
	BikeLanes->Move(MovementComponent->GetPreviousMovement());

	MoveNewLane(DeltaTime);
	float ForwardValue = SpeedBase + FMath::Clamp(PowerLevel / MAXPOWER, 0.f, 1.f) * SpeedMultiplier;
	FVector Direction = ForwardValue * GetActorForwardVector();
	MovementComponent->AddInputVector(Direction);
}

// Sets time for left input and calls AddTime
void ABikeCharacter::PedalLeftStart()
{
	TimeStartLeft = FPlatformTime::Seconds();
	AddTime();
}

// Sets time for right input and calls AddTime
void ABikeCharacter::PedalRightStart()
{
	TimeStartRight = FPlatformTime::Seconds();
	AddTime();
}

// Finds the absolute difference between the two inputs and adds to the PedalTimes array
// Calls CalculateBPM
void ABikeCharacter::AddTime()
{
	double TimeValue = abs(TimeStartLeft - TimeStartRight);

	// Adds the value to the array and checks to make sure the array is ARRAYMAXSIZE long
	PedalTimes.Add(TimeValue);
	if (PedalTimes.Num() > ARRAYMAXSIZE) PedalTimes.RemoveAt(0);

	CalculateBPM();
}

// Calculates the PowerLevel
void ABikeCharacter::CalculateBPM()
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

void ABikeCharacter::PowerTransition(float DeltaTime, float NewPower)
{
	if (PowerLevelTarget != NewPower)
	{
		PowerLevelCurrent = PowerLevelTarget;
		PowerLevelTarget = NewPower;
		PowerAlpha = 0;
	}

	PowerAlpha += DeltaTime / 3.f;
	PowerAlpha = FMath::Clamp(PowerAlpha, 0.f, 1.f);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Alpha: ") + FString::SanitizeFloat(PowerAlpha), true);
	PowerLevel = FMath::Lerp(PowerLevelCurrent, PowerLevelTarget, PowerAlpha);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Power Level: ") + FString::SanitizeFloat(PowerLevel), true);
}

void ABikeCharacter::MoveNewLane(float DeltaTime)
{
	FVector NewHorizontalPos = GetActorLocation();
	if (LaneSwitching || LaneBlocked)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Blocked"), true);
		switch (PowerLane)
		{
		case 0:
			NewHorizontalPos = BikeLanes->MoveLeft(false, DeltaTime);
			break;
		case 1:
			NewHorizontalPos = BikeLanes->MoveCenter(false, DeltaTime, GetActorLocation());
			break;
		case 2:
			NewHorizontalPos = BikeLanes->MoveRight(false, DeltaTime);
			break;
		default:
			break;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Not Blocked"), true);
		// Checks if PowerLevel is past the midpoint of the power scale
		if (PowerLevel > (UPPERPOWER + MIDDLEPOWER) / 2)
		{
			if (PowerLane == 0)
			{
				NewHorizontalPos = BikeLanes->MoveCenter(true, DeltaTime, GetActorLocation());
				PowerLane = 1;
			}
			else
			{
				if (PowerLane == 1) NewHorizontalPos = BikeLanes->MoveRight(true, DeltaTime);
				PowerLane = 2;
			}
		}
		else if (PowerLevel > (MIDDLEPOWER + LOWERPOWER) / 2)
		{
			if (PowerLane != 1) NewHorizontalPos = BikeLanes->MoveCenter(true, DeltaTime, GetActorLocation());
			PowerLane = 1;
		}
		else
		{
			if (PowerLane == 2)
			{
				NewHorizontalPos = BikeLanes->MoveCenter(true, DeltaTime, GetActorLocation());
				PowerLane = 1;
			}
			else
			{
				if (PowerLane == 1) NewHorizontalPos = BikeLanes->MoveLeft(true, DeltaTime);
				PowerLane = 0;
			}
		}
	}
	LaneSwitching = !BikeLanes->IsFinishedMove();
	
	NewHorizontalPos.Z = GetActorLocation().Z;
	SetActorLocation(NewHorizontalPos);
}

UBikeMovementComponent* ABikeCharacter::GetMovementComponent() const
{
	return MovementComponent;
}

void ABikeCharacter::Turn(float Angle)
{
	float NewAngle = GetActorRotation().Yaw + Angle;
	BikeLanes->Rotate(NewAngle);

	FRotator NewRotation = FRotator(GetActorRotation().Pitch, NewAngle, GetActorRotation().Roll);
	SetActorRotation(NewRotation);
}

float ABikeCharacter::GetPowerLevel() const
{
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Blue, TEXT("Power: ") + FString::SanitizeFloat(PowerLevel / MAXPOWER), true);
	return PowerLevel / MAXPOWER;
}

// Returns different power values based on Scale input
float ABikeCharacter::GetRawPower(int Scale) const
{
	switch (Scale)
	{
	case 0:
		return LOWERPOWER;
		break;
	case 1:
		return MIDDLEPOWER;
		break;
	case 2:
		return UPPERPOWER;
		break;
	case 3:
		return MAXPOWER;
		break;
	case 4:
	default:
		return PowerLevel;
		break;
	}
}

// Sets MAXPOWER from the GameInstance
void ABikeCharacter::LoadMaxPower()
{
	UBikeGameInstance* GameInstanceRef = Cast<UBikeGameInstance>(GetGameInstance());
	MAXPOWER = GameInstanceRef->GetMaxPower();

	// Sets three power stages to be a percentage of MAXPOWER
	UPPERPOWER = MAXPOWER * 0.7;
	MIDDLEPOWER = MAXPOWER * 0.5;
	LOWERPOWER = MAXPOWER * 0.3;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Easy: ") + FString::SanitizeFloat(LOWERPOWER), true);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Med: ") + FString::SanitizeFloat(MIDDLEPOWER), true);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Hard: ") + FString::SanitizeFloat(UPPERPOWER), true);
}

void ABikeCharacter::SetMaxPower(float NewPower)
{
	MAXPOWER = NewPower;
	// Sets three power stages to be a percentage of MAXPOWER
	UPPERPOWER = MAXPOWER * 0.7;
	MIDDLEPOWER = MAXPOWER * 0.5;
	LOWERPOWER = MAXPOWER * 0.3;
}

void ABikeCharacter::SetLanePos(FVector Easy, FVector Med, FVector Hard)
{
	EasyLanePos = Easy;
	MedLanePos = Med;
	HardLanePos = Hard;

	EasyLanePos.Z = GetActorLocation().Z;
	MedLanePos.Z = GetActorLocation().Z;
	HardLanePos.Z = GetActorLocation().Z;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Easy: ") + EasyLanePos.ToString(), true);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Med: ") + MedLanePos.ToString(), true);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Hard: ") + HardLanePos.ToString(), true);
}

void ABikeCharacter::SetLaneBlocked(bool Blocking)
{
	LaneBlocked = Blocking;
}

void ABikeCharacter::SetMovBlocked(bool Blocking)
{
	TutBlocked = Blocking;
}
