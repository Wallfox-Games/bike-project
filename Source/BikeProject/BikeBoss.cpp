// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeBoss.h"

#include "BikeProjectPlayerController.h"

// Sets default values
ABikeBoss::ABikeBoss()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthyCamTransform = FTransform(FRotator(5.f, -5.f, 0.0f), FVector(0.0f, 0.0f, 50.f), FVector(1.f));
	VulnerableCamTransform = FTransform(FRotator(-20.f, -120.0f, 0.0f), FVector(-50.0f, 0.0f, 20.f), FVector(1.f));

	HealthyCameraDistance = 900.f;
	VulnerableCameraDistance = 300.f;

	HealthyCameraFOV = 90.f;
	VulnerableCameraFOV = 60.f;

	// Our root component will be a sphere that reacts to physics
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(75.f, 35.f, 38.f));
	BoxComponent->SetCollisionProfileName(TEXT("Pawn"));

	BossDestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("BossDestructibleComponent"));
	BossDestructibleComponent->SetupAttachment(RootComponent);
	BossDestructibleComponent->SetRelativeTransform(FTransform(FRotator(0.f, 270.f, 0.f), FVector(20.f, 0.f, -40.f), FVector(1.f)));

	BossCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BossCameraSpringArm"));
	BossCameraSpringArm->SetupAttachment(RootComponent);

	BossCameraSpringArm->SetRelativeTransform(HealthyCamTransform);
	BossCameraSpringArm->TargetArmLength = HealthyCameraDistance;
	BossCameraSpringArm->bDoCollisionTest = false;

	BossCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BossCamera"));
	BossCamera->SetupAttachment(BossCameraSpringArm, USpringArmComponent::SocketName);

	BossCamera->SetFieldOfView(HealthyCameraFOV);

	MovementComponent = CreateDefaultSubobject<UBikeMovementComponent>(TEXT("BossMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	CameraLerpAlpha = 0.f;
}

void ABikeBoss::InitValues_Implementation(ABikeCharacter* NewPtr, int NewHealth, float NewSeconds, float NewMultiplier, float DeltaTime)
{
	PlayerPtr = NewPtr;
	Health = NewHealth;
	TargetSeconds = NewSeconds;
	TargetAttackPower = PlayerPtr->GetRawPower(3) * NewSeconds * NewMultiplier;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FText::Format(FTextFormat::FromString("Health: {0}\n Target Power: {1}"), Health, TargetAttackPower), true);

	SetActorRotation(PlayerPtr->GetActorRotation());
	FVector NewLocation = FVector(PlayerPtr->GetLaneActor()->GetActorLocation().X, PlayerPtr->GetLaneActor()->GetActorLocation().Y, PlayerPtr->GetActorLocation().Z - 25.f);
	SetActorLocation(NewLocation - (PlayerPtr->GetActorForwardVector() * 400.f));

	BossStateEnum = BSE_Moving;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoFail = true;
	SpawnInfo.Owner = this;
	BikeLanes = GetWorld()->SpawnActor<ABikeLaneActor>(GetActorLocation(), PlayerPtr->GetActorRotation(), SpawnInfo);

	BikeLanes->InitValues(PlayerPtr->GetLaneWidth(), 1.8f);
	SetActorLocation(GetActorLocation() + (GetActorRightVector() * PlayerPtr->GetLaneWidth()));
	CurrentLane = 2;

	ABikeProjectPlayerController* PlayerControllerPtr = Cast<ABikeProjectPlayerController>(PlayerPtr->GetController());
	PlayerControllerPtr->SetMoveEnum(PME_LaneBoss, DeltaTime);
}

// Called when the game starts or when spawned
void ABikeBoss::BeginPlay()
{
	Super::BeginPlay();
}

void ABikeBoss::Movement()
{
	BikeLanes->Move(MovementComponent->GetPreviousMovement());

	FVector MoveVec = PlayerPtr->GetPrevMov();

	switch (BossStateEnum)
	{
	case BSE_Moving:
		MoveVec += (0.5f * PlayerPtr->GetPrevMov());
		break;
	case BSE_Healthy:
		break;
	case BSE_Vulnerable:
		MoveVec -= (0.3f * PlayerPtr->GetPrevMov());
		break;
	default:
		break;
	}

	MovementComponent->AddInputVector(MoveVec);
}

void ABikeBoss::SetCameraPosition(float DeltaTime)
{
	if (BossStateEnum != BSE_Vulnerable && CameraLerpAlpha != 0.f)
	{
		CameraLerpAlpha -= DeltaTime;
	}
	else if (BossStateEnum == BSE_Vulnerable && CameraLerpAlpha != 1.f)
	{
		CameraLerpAlpha += DeltaTime;
	}
	CameraLerpAlpha = FMath::Clamp(CameraLerpAlpha, 0.f, 1.f);

	FTransform NewCameraTransform;
	NewCameraTransform.Blend(HealthyCamTransform, VulnerableCamTransform, CameraLerpAlpha);

	float NewCameraDist = FMath::Lerp(HealthyCameraDistance, VulnerableCameraDistance, CameraLerpAlpha);
	float NewCameraFOV = FMath::Lerp(HealthyCameraFOV, VulnerableCameraFOV, CameraLerpAlpha);

	BossCameraSpringArm->SetRelativeTransform(NewCameraTransform);
	BossCameraSpringArm->TargetArmLength = NewCameraDist;
	BossCamera->SetFieldOfView(NewCameraFOV);
}

// Called every frame
void ABikeBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Movement();

	ABikeProjectPlayerController* PlayerControllerPtr = Cast<ABikeProjectPlayerController>(PlayerPtr->GetController());

	if (BossStateEnum == BSE_Moving)
	{
		float ToPlayerDist = FVector::Distance(GetActorLocation(), PlayerPtr->GetActorLocation());
		if (ToPlayerDist >= 500.f && ToPlayerDist < 800.f)
		{
			if (CurrentLane != 1)
			{
				SetActorLocation(BikeLanes->MoveCenter(true, DeltaTime, GetActorLocation()));
				CurrentLane = 1;
				LaneChange = true;
			}
			else if (LaneChange)
			{
				SetActorLocation(BikeLanes->MoveCenter(false, DeltaTime, GetActorLocation()));
				LaneChange = !BikeLanes->IsFinishedMove();
			}
			if (PlayerControllerPtr->GetViewTarget() != this)
			{
				PlayerControllerPtr->SetViewTargetWithBlend(this, 1.f);
			}
		}
		else if (ToPlayerDist >= 800.f)
		{
			BossStateEnum = BSE_Healthy;
			CanHit = true;
		}
	}

	else if (BossStateEnum == BSE_Healthy && !PlayerControllerPtr->GetMoveUIBlocked() && !PlayerControllerPtr->GetMovePauseBlocked())
	{
		CurrentAttackPower += FMath::Clamp(PlayerPtr->GetRawPower(4), 0.f, PlayerPtr->GetRawPower(3)) * DeltaTime;
		CurrentTime += DeltaTime;

		if (CurrentAttackPower > TargetAttackPower)
		{
			BossStateEnum = EBossState::BSE_Vulnerable;
			PlayerControllerPtr->SetMoveEnum(PME_AttackBoss, DeltaTime);
		}
		else if (CurrentTime >= TargetSeconds)
		{
			CurrentAttackPower = 0;
			CurrentTime = 0;
			PlayerControllerPtr->PlayerHit(true);
		}
	}

	SetCameraPosition(DeltaTime);
}

float ABikeBoss::GetCurrentAttackPower() const
{
	return CurrentAttackPower;
}

float ABikeBoss::GetPercentageAttackPower() const
{
	return CurrentAttackPower / TargetAttackPower;
}

float ABikeBoss::GetCurrentTime() const
{
	return CurrentTime;
}

float ABikeBoss::GetPercentageTime() const
{
	return CurrentTime / TargetSeconds;
}

float ABikeBoss::GetTimeToGo() const
{
	return TargetSeconds - CurrentTime;
}

int ABikeBoss::GetHealth() const
{
	return Health;
}

EBossState ABikeBoss::GetBossEnum() const
{
	return BossStateEnum;
}

bool ABikeBoss::OnHit_Implementation()
{
	if (CanHit)
	{
		ABikeProjectPlayerController* PlayerControllerPtr = Cast<ABikeProjectPlayerController>(PlayerPtr->GetController());
		PlayerControllerPtr->SetMoveEnum(PME_LaneBoss, 0.f);
		Health--;
		CanHit = false;

		if (Health > 0)
		{
			CurrentAttackPower = 0;
			CurrentTime = 0;
			BossStateEnum = BSE_Moving;
			return false;
		}
		else
		{
			BossStateEnum = BSE_Defeated;
			return true;
		}
	}
	return false;
}