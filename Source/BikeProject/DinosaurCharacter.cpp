// Fill out your copyright notice in the Description page of Project Settings.


#include "DinosaurCharacter.h"

// Sets default values
ADinosaurCharacter::ADinosaurCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADinosaurCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADinosaurCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADinosaurCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

