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

	// Sets default lane to middle lane
	PowerLane = 1;
	// Sets the starting values for non bike input to current time, so the first input isn't very long
	TimeStartLeft = FPlatformTime::Seconds();
	TimeStartRight = FPlatformTime::Seconds();
	// Reserves memory for the pedal array
	PedalTimes.Reserve(ARRAYMAXSIZE + 1);

	// Default variables for lanes and width (editable in blueprints)
	LaneSpeed = 1.5f;
	SpeedBase = 200.f;
	SpeedMultiplier = 300.f;
	LaneBlocked = false;
	LaneSwitching = false;

	//SetLanePos(
		//FVector(GetActorLocation().X, GetActorLocation().Y - 90, GetActorLocation().Z),
		//GetActorLocation(),
		//FVector(GetActorLocation().X, GetActorLocation().Y + 90, GetActorLocation().Z)
	//);
}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetMaxPower();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Biker is being used."));
}

// Called every frame
void ABikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UBikeGameInstance* GameInstanceRef = Cast<UBikeGameInstance>(GetGameInstance());
	PowerLevelBP = GameInstanceRef->GetSpeed();

	if (PowerLevelKB > 0) PowerLevel = PowerLevelKB;
	else PowerLevel = PowerLevelBP;

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Bike Input Speed: ") + FString::SanitizeFloat(PowerLevel), true);

	// Determine current Lane
	// Snap to Upper and Lower Lanes
	Movement(DeltaTime);
}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAction("PedalLeftButton", IE_Pressed, this, &ABikeCharacter::PedalLeftStart);
	PlayerInputComponent->BindAction("PedalRightButton", IE_Pressed, this, &ABikeCharacter::PedalRightStart);

	// Set up "test" bindings.
	PlayerInputComponent->BindAction("RotateLeft", IE_Pressed, this, &ABikeCharacter::RotateLeft);
	PlayerInputComponent->BindAction("RotateRight", IE_Pressed, this, &ABikeCharacter::RotateRight);
}

// Checks current power of dinosaur and moves them into correct lane
// Moves dinosaur forward at a speed value relative to power
void ABikeCharacter::Movement(float DeltaTime)
{
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

	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Keyboard Input Power: ") + FString::SanitizeFloat(PowerLevel), true);
}

void ABikeCharacter::MoveNewLane(float DeltaTime)
{
	float horVal = 0;
	if (LaneSwitching || LaneBlocked)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Blocked"), true);
		switch (PowerLane)
		{
		case 0:
			horVal = MoveEasyDT(DeltaTime);
			break;
		case 1:
			horVal = MoveMedDT(DeltaTime);
			break;
		case 2:
			horVal = MoveHardDT(DeltaTime);
			break;
		default:
			break;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Not Blocked"), true);
		// Checks if PowerLevel is past the midpoint of the power scale
		if (PowerLevel > (UPPERPOWER + MIDDLEPOWER) / 2) horVal = MoveHardDT(DeltaTime);
		else if (PowerLevel > (MIDDLEPOWER + LOWERPOWER) / 2) horVal = MoveMedDT(DeltaTime);
		else horVal = MoveEasyDT(DeltaTime);
	}
	if (horVal != 0) LaneSwitching = true;
	else if (horVal == 0) LaneSwitching = false;
	
	SetActorLocation(GetActorLocation() + GetActorRightVector() * horVal * LaneSpeed);
}

// Smoothly moves the player into the hard lane
float ABikeCharacter::MoveHardDT(float DeltaTime)
{	
	PowerLane = 2;
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Hard Lane"), true);
	
	FVector ToLane = GetActorLocation() - HardLanePos;
	ToLane.Normalize();
	ToLane.Z = GetActorForwardVector().Z;

	float dotAngle = 1 - FVector::DotProduct(GetActorForwardVector(), ToLane);
	float crossAngle = FVector::CrossProduct(GetActorForwardVector(), ToLane).Z;

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Dot Angle: ") + FString::SanitizeFloat(dotAngle), true);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Cross Angle: ") + FString::SanitizeFloat(crossAngle), true);
	if (dotAngle < 0.05) return 0;

	if (crossAngle > 0) return dotAngle * -1;
	return dotAngle;
}

// Smoothly moves the player into the medium lane
float ABikeCharacter::MoveMedDT(float DeltaTime)
{
	PowerLane = 1;
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Hard Lane"), true);

	FVector ToLane = GetActorLocation() - MedLanePos;
	ToLane.Normalize();
	ToLane.Z = GetActorForwardVector().Z;

	float dotAngle = 1 - FVector::DotProduct(GetActorForwardVector(), ToLane);
	float crossAngle = FVector::CrossProduct(GetActorForwardVector(), ToLane).Z;

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Dot Angle: ") + FString::SanitizeFloat(dotAngle), true);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Cross Angle: ") + FString::SanitizeFloat(crossAngle), true);
	if (dotAngle < 0.05) return 0;

	if (crossAngle > 0) return dotAngle * -1;
	return dotAngle;
}

// Smoothly moves the player into the easy lane
float ABikeCharacter::MoveEasyDT(float DeltaTime)
{
	PowerLane = 0;
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Hard Lane"), true);

	FVector ToLane = GetActorLocation() - EasyLanePos;
	ToLane.Normalize();
	ToLane.Z = GetActorForwardVector().Z;

	float dotAngle = 1 - FVector::DotProduct(GetActorForwardVector(), ToLane);
	float crossAngle = FVector::CrossProduct(GetActorForwardVector(), ToLane).Z;

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Dot Angle: ") + FString::SanitizeFloat(dotAngle), true);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Cross Angle: ") + FString::SanitizeFloat(crossAngle), true);
	if (dotAngle < 0.05) return 0;

	if (crossAngle > 0) return dotAngle * -1;
	return dotAngle;
}

UBikeMovementComponent* ABikeCharacter::GetMovementComponent() const
{
	return MovementComponent;
}

void ABikeCharacter::TurnActor(float Angle)
{
	float NewAngle = GetActorRotation().Yaw + Angle;
	FRotator NewRotation = FRotator(GetActorRotation().Pitch, NewAngle, GetActorRotation().Roll);
	SetActorRotation(NewRotation);
}

void ABikeCharacter::RotateLeft()
{
	TurnActor(-10.f);
}

void ABikeCharacter::RotateRight()
{
	TurnActor(10.f);
}

float ABikeCharacter::GetPowerLevel() const
{
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
void ABikeCharacter::SetMaxPower()
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
