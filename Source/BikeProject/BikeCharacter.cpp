// Fill out your copyright notice in the Description page of Project Settings.


#include "BikeCharacter.h"

// Sets default values
ABikeCharacter::ABikeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a third person camera component.
	TPCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	check(TPCameraComponent != nullptr);

	// Attach the camera component to our capsule component.
	TPCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	// Position the camera behind the pawn.
	TPCameraComponent->SetRelativeLocation(FVector(-120.0f, 0.0f, 80.0f + BaseEyeHeight));

	// Enable the pawn to control camera rotation.
	TPCameraComponent->bUsePawnControlRotation = true;
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

}

// Called to bind functionality to input
void ABikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("Move", this, &ABikeCharacter::Movement);

	// Set up "power" bindings.
	PlayerInputComponent->BindAxis("Power", this, &ABikeCharacter::PowerDisplay);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &ABikeCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ABikeCharacter::AddControllerPitchInput);
}

void ABikeCharacter::Movement(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);

	Value = FMath::Clamp(Value, -1.0f, 1.0f);
	AddMovementInput(Direction, Value);
}

void ABikeCharacter::PowerDisplay(float Value)
{
	powerLevel = Value;
}

float ABikeCharacter::GetPowerLevel() const
{
	return powerLevel;
}
