// Fill out your copyright notice in the Description page of Project Settings.

#include "BikeBoss.h"

#include "BikeProjectPlayerController.h"
#include "BikeGameInstance.h"

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

	BossVisibleComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BossVisibleComponent"));
	BossVisibleComponent->SetupAttachment(RootComponent);
	BossVisibleComponent->SetRelativeTransform(FTransform(FRotator(0.f, 270.f, 0.f), FVector(20.f, 0.f, -40.f), FVector(1.f)));
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

	PlayerPtr = nullptr;
	BikeLanes = nullptr;

	Cooldown = 0.f;
	ObstacleCurrent = -1;
	ObstacleTick = 0.f;
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

	ChangeState(BSE_Moving);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoFail = true;
	SpawnInfo.Owner = this;
	BikeLanes = GetWorld()->SpawnActor<ABikeLaneActor>(GetActorLocation(), PlayerPtr->GetActorRotation(), SpawnInfo);

	BikeLanes->InitValues(PlayerPtr->GetLaneWidth(), 1.8f);
	SetActorLocation(GetActorLocation() + (GetActorRightVector() * PlayerPtr->GetLaneWidth()));
	CurrentLane = 2;

	ABikeProjectPlayerController* PlayerControllerPtr = Cast<ABikeProjectPlayerController>(PlayerPtr->GetController());
	PlayerControllerPtr->SetMoveEnum(PME_BossCooldown, DeltaTime);
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
	case BSE_Vulnerable:
		MoveVec -= (0.3f * PlayerPtr->GetPrevMov());
		break;
	default:
		break;
	}

	MovementComponent->AddInputVector(MoveVec);
}

void ABikeBoss::SetCameraPosition(float DeltaTime, ABikeProjectPlayerController* PlayerControllerPtr)
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

	FVector NewHorizontalPos = GetActorLocation();

	ABikeProjectPlayerController* PlayerControllerPtr = Cast<ABikeProjectPlayerController>(PlayerPtr->GetController());
	float ToPlayerDist = FVector::Distance(GetActorLocation(), PlayerPtr->GetActorLocation());

	if (!PlayerControllerPtr->GetMoveUIBlocked() && !PlayerControllerPtr->GetMovePauseBlocked())
	{
		switch (BossStateEnum)
		{
		case BSE_Moving:
			if (ToPlayerDist >= 500.f && ToPlayerDist < 800.f)
			{
				if (CurrentLane != 1)
				{
					NewHorizontalPos = BikeLanes->MoveCenter(true, DeltaTime, GetActorLocation());
					CurrentLane = 1;
					LaneChange = true;
				}
				else if (LaneChange)
				{
					NewHorizontalPos = BikeLanes->MoveCenter(false, DeltaTime, GetActorLocation());
					LaneChange = !BikeLanes->IsFinishedMove();
				}
			}
			else if (ToPlayerDist >= 800.f)
			{
				ChangeState(BSE_Cooldown);
				PlayerControllerPtr->SetMoveEnum(PME_BossCooldown, DeltaTime);
			}
			Cooldown = FMath::Clamp(Cooldown - DeltaTime, 0.f, MaxCooldown);
			break;

		case BSE_Cooldown:
			if (Cooldown == 0.f)
			{
				CanHit = true;
				ObstacleStringTemp = ObstacleString;
				ChangeState(BSE_Attacking);
				PlayerControllerPtr->SetMoveEnum(PME_BossDodge, DeltaTime);
			}
			else
			{
				Cooldown = FMath::Clamp(Cooldown - DeltaTime, 0.f, MaxCooldown);
			}
			break;

		case BSE_Attacking:
			if (ObstacleTick == 0.f)
			{
				if (ObstacleCurrent == -1)
				{
					if (ObstacleStringTemp.Len() > 0)
					{
						ObstacleCurrent = FCString::Atoi(*(ObstacleStringTemp.Left(1)));
						ObstacleStringTemp.RemoveAt(0);
						if (ObstacleCurrent == 3) ObstacleTick = ObstacleMaxTick;
						else if (CurrentLane != ObstacleCurrent)
						{
							switch (ObstacleCurrent)
							{
							case 0:
								NewHorizontalPos = BikeLanes->MoveLeft(true, DeltaTime);
								CurrentLane = 0;
								break;
							case 1:
								NewHorizontalPos = BikeLanes->MoveCenter(true, DeltaTime, GetActorLocation());
								CurrentLane = 1;
								break;
							case 2:
								NewHorizontalPos = BikeLanes->MoveRight(true, DeltaTime);
								CurrentLane = 2;
								break;
							default:
								break;
							}
							LaneChange = !BikeLanes->IsFinishedMove();
						}
						else SpawnMine();
					}
					else
					{
						ObstacleCurrent = -1;
						CurrentAttackPower = 0;
						CurrentTime = 0;
						ChangeState(BSE_Despawning);
						PlayerControllerPtr->SetMoveEnum(PME_BossCharge, DeltaTime);
					}
				}
				else if (LaneChange)
				{
					switch (CurrentLane)
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
					LaneChange = !BikeLanes->IsFinishedMove();
				}

				if (BossStateEnum == BSE_Attacking)
				{
					if (ObstacleCurrent == 3) ObstacleCurrent = -1;
					else if (!LaneChange) SpawnMine();
				}
			}
			else ObstacleTick = FMath::Clamp(ObstacleTick - DeltaTime, 0.f, ObstacleMaxTick);
			break;

		case BSE_Despawning:
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Current Lane: ") + FString::FromInt(CurrentLane), true);
			if (CurrentLane != 1)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Start Switching Lanes"), true);
				NewHorizontalPos = BikeLanes->MoveCenter(true, DeltaTime, GetActorLocation());
				CurrentLane = 1;
				LaneChange = true;
			}
			else if (LaneChange)
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Switching Lanes"), true);
				NewHorizontalPos = BikeLanes->MoveCenter(false, DeltaTime, GetActorLocation());
				LaneChange = !BikeLanes->IsFinishedMove();
			}
			else if (ObstaclesDestroyed)
			{
				ChangeState(EBossState::BSE_Reloading);
			}
			break;

		case BSE_Reloading:
			CurrentAttackPower += FMath::Clamp(PlayerPtr->GetRawPower(4), 0.f, PlayerPtr->GetRawPower(3)) * DeltaTime;
			CurrentTime += DeltaTime;

			if (CurrentAttackPower > TargetAttackPower)
			{
				ChangeState(EBossState::BSE_Vulnerable);
				PlayerControllerPtr->SetMoveEnum(PME_BossAttack, DeltaTime);
			}
			else if (CurrentTime >= TargetSeconds)
			{
				Cooldown = MaxCooldown;
				ChangeState(EBossState::BSE_Cooldown);
				PlayerControllerPtr->SetMoveEnum(PME_BossCooldown, DeltaTime);
			}
			break;

		case BSE_Vulnerable:
			break;

		case BSE_Defeated:
		default:
			break;
		}

		NewHorizontalPos.Z = GetActorLocation().Z;
		SetActorLocation(NewHorizontalPos);

		SetCameraPosition(DeltaTime, PlayerControllerPtr);
	}
}

void ABikeBoss::ChangeState_Implementation(EBossState NewState)
{
	BossStateEnum = NewState;

	ABikeProjectPlayerController* PlayerControllerPtr = Cast<ABikeProjectPlayerController>(PlayerPtr->GetController());
	UBikeGameInstance* GameInstanceRef = Cast<UBikeGameInstance>(GetGameInstance());

	switch (NewState)
	{
	case BSE_Moving:
		break;
	case BSE_Cooldown:
		break;
	case BSE_Attacking:
		break;
	case BSE_Despawning:

		if (PlayerControllerPtr->GetViewTarget() != this)
		{
			PlayerControllerPtr->SetViewTargetWithBlend(this, 1.f);
		}
		break;
	case BSE_Reloading:

		if (PlayerControllerPtr->GetViewTarget() != this)
		{
			PlayerControllerPtr->SetViewTargetWithBlend(this, 1.f);
		}
		break;
	case BSE_Vulnerable:

		if (PlayerControllerPtr->GetViewTarget() != this)
		{
			PlayerControllerPtr->SetViewTargetWithBlend(this, 1.f);
		}
		break;
	case BSE_Defeated:
		GameInstanceRef->SetMaxPower(PlayerControllerPtr->GetPowerLevelMax());
		break;
	default:
		break;
	}
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

void ABikeBoss::SetObstaclesDestroyed(bool IsDestroyed)
{
	ObstaclesDestroyed = IsDestroyed;
}

void ABikeBoss::SpawnMine_Implementation()
{
	ObstacleCurrent = -1;
	ObstacleTick = ObstacleMaxTick;
	ObstaclesDestroyed = false;
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
		Health--;
		CanHit = false;

		if (Health > 0)
		{
			ChangeState(BSE_Moving);
			PlayerControllerPtr->SetMoveEnum(PME_BossCooldown, 0.f);
			Cooldown = MaxCooldown;
			return false;
		}
		else
		{
			ChangeState(BSE_Defeated);
			PlayerControllerPtr->SetMoveEnum(PME_Normal, 0.f);
			return true;
		}
	}
	return false;
}