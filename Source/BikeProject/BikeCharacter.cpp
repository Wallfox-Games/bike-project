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
	CapsuleComponent->SetCapsuleRadius(50);
	CapsuleComponent->SetCapsuleHalfHeight(50);
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
	LaneWidth = 80.f;
	LaneSpeed = 1.5f;
	SpeedBase = 200.f;
	SpeedMultiplier = 300.f;
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

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Bike Input Speed: ") + FString::SanitizeFloat(PowerLevel), true);

	if (PowerLevelKB > 0) PowerLevel = PowerLevelKB;
	else PowerLevel = PowerLevelBP;

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
	//if (!TutorialState)
	//{
		// Checks if PowerLevel is past the midpoint of the power scale
		if (PowerLevel > (UPPERPOWER + MIDDLEPOWER) / 2) MoveHardDT(DeltaTime);
		else if (PowerLevel > (MIDDLEPOWER + LOWERPOWER) / 2) MoveMedDT(DeltaTime);
		else MoveEasyDT(DeltaTime);
	//}

	// Find out which way is "forward" and record that the player wants to move that way.
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

// Smoothly moves the player into the hard lane
void ABikeCharacter::MoveHardDT(float DeltaTime)
{	
	float Position = GetActorLocation().Y;
	float HorizontalValue;

	// if the Position is close to the LaneWidth, then snap the value to the LaneWidth
	if (Position > (0.95f * LaneWidth)) HorizontalValue = LaneWidth;
	// else, lerp the value towards the LaneWidth
	else HorizontalValue = FMath::Lerp(Position, LaneWidth, DeltaTime * LaneSpeed);

	// set the actor to the new Y value
	FVector newLocation = FVector(GetActorLocation().X, HorizontalValue, GetActorLocation().Z);
	SetActorLocation(newLocation);

	// if the new value has entered the new lane, change the PowerLane variable
	if (HorizontalValue > (0.7f * LaneWidth)) {
		PowerLane = 2;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Hard lane"));
	}
}

// Smoothly moves the player into the medium lane
void ABikeCharacter::MoveMedDT(float DeltaTime)
{
	float Position = GetActorLocation().Y;
	float HorizontalValue;

	// if the Position is close to the LaneWidth, then snap the value to the LaneWidth
	if (abs(Position) < (0.05f * LaneWidth)) HorizontalValue = 0.f;
	// else, lerp the value towards the LaneWidth
	else HorizontalValue = FMath::Lerp(Position, 0.f, DeltaTime * LaneSpeed);

	// set the actor to the new Y value
	FVector newLocation = FVector(GetActorLocation().X, HorizontalValue, GetActorLocation().Z);
	SetActorLocation(newLocation);

	// if the new value has entered the new lane, change the PowerLane variable
	if (abs(HorizontalValue) < (0.3f * LaneWidth) && abs(HorizontalValue) > (0.3f * -LaneWidth)) {
		PowerLane = 1;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Medium lane"));
	}
}

// Smoothly moves the player into the easy lane
void ABikeCharacter::MoveEasyDT(float DeltaTime)
{
	float Position = GetActorLocation().Y;
	float HorizontalValue;

	// if the Position is close to the LaneWidth, then snap the value to the LaneWidth
	if (Position < (0.95f * -LaneWidth)) HorizontalValue = -LaneWidth;
	// else, lerp the value towards the LaneWidth
	else HorizontalValue = FMath::Lerp(Position, -LaneWidth, DeltaTime * LaneSpeed);

	// set the actor to the new Y value
	FVector newLocation = FVector(GetActorLocation().X, HorizontalValue, GetActorLocation().Z);
	SetActorLocation(newLocation);

	// if the new value has entered the new lane, change the PowerLane variable
	if (HorizontalValue < (0.7f * -LaneWidth)) {
		PowerLane = 0;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Easy lane"));
	}
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
	UPPERPOWER = MAXPOWER * 0.8;
	MIDDLEPOWER = MAXPOWER * 0.65;
	LOWERPOWER = MAXPOWER * 0.5;
}