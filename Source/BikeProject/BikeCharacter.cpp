// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeCharacter.h"
#include "BikeProject.h"

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

	PowerLane = 1;
	TimeStartLeft = FPlatformTime::Seconds();
	TimeStartRight = FPlatformTime::Seconds();
	PedalTimes.Reserve(ARRAYMAXSIZE + 1);

	// Only load game stats if the load .sav file exists
	const FString SaveSlotName = FString(TEXT("PlayerSaveSlot"));

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		class UBikeProjectSaveGame* LoadInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UBikeProjectSaveGame::StaticClass()));
		LoadInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance->SaveSlotName, LoadInstance->UserIndex));
		MAXPOWER = LoadInstance->PlayerMaxPower;
		TutorialState = false;
	}
	else
	{
		// In tutorial mode
		MAXPOWER = 500;
		TutorialState = true;
	}

	UPPERPOWER = MAXPOWER * 0.8;
	MIDDLEPOWER = MAXPOWER * 0.65;
	LOWERPOWER = MAXPOWER * 0.5;

	LaneWidth = 80.f;
	LaneSpeed = 1.5f;
	SpeedBase = 200.f;
	SpeedMultiplier = 300.f;
}

// Called when the game starts or when spawned
void ABikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Biker is being used."));
}

// Called every frame
void ABikeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Determine current Lane
	// Snap to Upper and Lower Lanes
	Movement(DeltaTime);
}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAction("PowerLevelUp", IE_Pressed, this, &ABikeCharacter::MoveUp);
	PlayerInputComponent->BindAction("PowerLevelDown", IE_Pressed, this, &ABikeCharacter::MoveDown);

	PlayerInputComponent->BindAction("PedalLeftButton", IE_Pressed, this, &ABikeCharacter::PedalLeftStart);
	PlayerInputComponent->BindAction("PedalRightButton", IE_Pressed, this, &ABikeCharacter::PedalRightStart);

	PlayerInputComponent->BindAction("PedalLeftButton", IE_Released, this, &ABikeCharacter::PedalLeftEnd);
	PlayerInputComponent->BindAction("PedalRightButton", IE_Released, this, &ABikeCharacter::PedalRightEnd);

	PlayerInputComponent->BindAxis("PedalLeftAxis", this, &ABikeCharacter::PedalLeftAxis);
	PlayerInputComponent->BindAxis("PedalRightAxis", this, &ABikeCharacter::PedalRightAxis);
}

void ABikeCharacter::Movement(float DeltaTime)
{
	if (PowerLevel > (UPPERPOWER + MIDDLEPOWER) / 2) MoveHardDT(DeltaTime);
	else if (PowerLevel > (MIDDLEPOWER + LOWERPOWER) / 2) MoveMedDT(DeltaTime);
	else MoveEasyDT(DeltaTime);

	// Find out which way is "forward" and record that the player wants to move that way.
	float ForwardValue = SpeedBase + FMath::Clamp(PowerLevel / MAXPOWER, 0.f, 1.f) * SpeedMultiplier;
	FVector Direction = ForwardValue * GetActorForwardVector();;
	MovementComponent->AddInputVector(Direction);
}

void ABikeCharacter::MoveUp()
{
	if (PowerLane < 2) PowerLane++;
	if (PowerLane == 2)
	{
		MoveHard();
	}
	else
	{
		MoveMed();
	}
}

void ABikeCharacter::MoveDown()
{
	if (PowerLane > 0) PowerLane--;
	if (PowerLane == 0)
	{
		MoveEasy();
	}
	else
	{
		MoveMed();
	}
}

void ABikeCharacter::PedalLeftStart()
{
	PedalLeft = true;
	TimeStartLeft = FPlatformTime::Seconds();
	AddTime();
}

void ABikeCharacter::PedalRightStart()
{
	PedalRight = true;
	TimeStartRight = FPlatformTime::Seconds();
	AddTime();
}

void ABikeCharacter::PedalLeftEnd()
{
	PedalLeft = false;
}

void ABikeCharacter::PedalRightEnd()
{
	PedalRight = false;
}

void ABikeCharacter::PedalLeftAxis(float Value)
{
	if (!PedalLeft && Value > 0.7) PedalLeftStart();
	else if (PedalLeft && Value <= 0.7) PedalLeftEnd();
}

void ABikeCharacter::PedalRightAxis(float Value)
{
	if (!PedalRight && Value > 0.7) PedalRightStart();
	else if (PedalRight && Value <= 0.7) PedalRightEnd();
}

void ABikeCharacter::AddTime()
{
	double TimeValue = abs(TimeStartLeft - TimeStartRight);

	PedalTimes.Add(TimeValue);
	if (PedalTimes.Num() > ARRAYMAXSIZE) PedalTimes.RemoveAt(0);

	CalculateBPM();
}

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
	PowerLevelOld = PowerLevel;
	PowerLevel = RPM / 2;

	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Blue, TEXT("RPM: ") + FString::SanitizeFloat(RPM), true);
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, TEXT("Power: ") + FString::SanitizeFloat(PowerLevel), true);
}

void ABikeCharacter::MoveHard()
{
	FVector newLocation = FVector(GetActorLocation().X, -LaneWidth, GetActorLocation().Z);
	SetActorLocation(newLocation);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Hard lane"));
}

void ABikeCharacter::MoveMed()
{
	FVector newLocation = FVector(GetActorLocation().X, 0.f, GetActorLocation().Z);
	SetActorLocation(newLocation);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Medium lane"));
}

void ABikeCharacter::MoveEasy()
{
	FVector newLocation = FVector(GetActorLocation().X, LaneWidth, GetActorLocation().Z);
	SetActorLocation(newLocation);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Easy lane"));
}

void ABikeCharacter::MoveHardDT(float DeltaTime)
{
	float HorizontalValue;
	if (GetActorLocation().Y < (0.95f * -LaneWidth)) HorizontalValue = -LaneWidth;
	else HorizontalValue = FMath::Lerp(GetActorLocation().Y, -LaneWidth, DeltaTime * LaneSpeed);
	FVector newLocation = FVector(GetActorLocation().X, HorizontalValue, GetActorLocation().Z);
	SetActorLocation(newLocation);
	if (GetActorLocation().Y < (0.7f * -LaneWidth)) {
		PowerLane = 2;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Hard lane"));
	}
}

void ABikeCharacter::MoveMedDT(float DeltaTime)
{
	float HorizontalValue;
	if (abs(GetActorLocation().Y) < (0.05f * LaneWidth)) HorizontalValue = 0.f;
	else HorizontalValue = FMath::Lerp(GetActorLocation().Y, 0.f, DeltaTime * LaneSpeed);
	FVector newLocation = FVector(GetActorLocation().X, HorizontalValue, GetActorLocation().Z);
	SetActorLocation(newLocation);
	if (abs(GetActorLocation().Y) < (0.3f * LaneWidth) && abs(GetActorLocation().Y) > (0.3f * -LaneWidth)) {
		PowerLane = 1;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Medium lane"));
	}
}

void ABikeCharacter::MoveEasyDT(float DeltaTime)
{
	float HorizontalValue;
	if (GetActorLocation().Y > (0.95f * LaneWidth)) HorizontalValue = LaneWidth;
	else HorizontalValue = FMath::Lerp(GetActorLocation().Y, LaneWidth, DeltaTime * LaneSpeed);
	FVector newLocation = FVector(GetActorLocation().X, HorizontalValue, GetActorLocation().Z);
	SetActorLocation(newLocation);
	if (GetActorLocation().Y > (0.7f * LaneWidth)) {
		PowerLane = 0;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Easy lane"));
	}
}

UBikeMovementComponent* ABikeCharacter::GetMovementComponent() const
{
	return MovementComponent;
}

bool ABikeCharacter::GetTutorialState() const
{
	return TutorialState;
}

float ABikeCharacter::GetPowerLevel() const
{
	return PowerLevel / MAXPOWER;
}

float ABikeCharacter::GetRawPower() const
{
	return PowerLevel;
}

void ABikeCharacter::SetMaxPower(float newMaxPower)
{
	MAXPOWER = newMaxPower;

	if (UBikeProjectSaveGame* SaveGameInstance = Cast<UBikeProjectSaveGame>(UGameplayStatics::CreateSaveGameObject(UBikeProjectSaveGame::StaticClass())))
	{
		// Set data on the savegame object.
		SaveGameInstance->PlayerMaxPower = MAXPOWER;

		// Save the data immediately.
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex))
		{
			// Save succeeded.
		}
	}
}

