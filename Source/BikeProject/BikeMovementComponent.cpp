// Fill out your copyright notice in the Description page of Project Settings.


#include "BikeMovementComponent.h"

void UBikeMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Make sure that everything is still valid, and that we are allowed to move.
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    FVector DesiredMovementThisFrame = FVector(0.f, 0.f, -30.f) * DeltaTime;
    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);
    }

    // Get (and then clear) the movement vector that we set in ABikeCharacter::Tick
    DesiredMovementThisFrame = ConsumeInputVector() * DeltaTime;
    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        FHitResult Hit;
        if (SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit)) PrevMove = DesiredMovementThisFrame;
        else
        {
            DesiredMovementThisFrame += FVector(0.f, 0.f, 1.f) * DeltaTime;
            if (SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit)) PrevMove = DesiredMovementThisFrame;
            else PrevMove = FVector(0.f);
        }
    }
    else
    {
        PrevMove = FVector(0.f);
    }
};

FVector UBikeMovementComponent::GetPreviousMovement()
{
    return PrevMove;
}
