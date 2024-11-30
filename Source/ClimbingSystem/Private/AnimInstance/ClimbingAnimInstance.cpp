// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingSystem/Public/AnimInstance/ClimbingAnimInstance.h"
#include "ClimbingSystem/ClimbingSystemCharacter.h"
#include "ClimbingSystem/CustomComponents/CustomCharacterMovementComponent.h"

void UClimbingAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ClimbingSystemCharacter = Cast<AClimbingSystemCharacter>(TryGetPawnOwner());
	{
		if (ClimbingSystemCharacter)
		{
			CustomCharacterMovementComponent = ClimbingSystemCharacter->GetCustomMovementComponent();
		}
	}
}

void UClimbingAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CustomCharacterMovementComponent) return;

	GetGroundSpeed();
	GetIsFalling();
	GetAirSpeed();
	GetShouldMove();	
	GetIsClimbing();
	GetClimbVelocity();
}

void UClimbingAnimInstance::GetGroundSpeed()
{
	GroundSpeed = CustomCharacterMovementComponent->Velocity.Size2D();
}

void UClimbingAnimInstance::GetIsFalling()
{
	bIsFalling = CustomCharacterMovementComponent->IsFalling();
}

void UClimbingAnimInstance::GetAirSpeed()
{
	AirSpeed = CustomCharacterMovementComponent->Velocity.Z;
}

void UClimbingAnimInstance::GetShouldMove()
{
	bShouldMove = CustomCharacterMovementComponent->GetCurrentAcceleration().Size()>0 && GroundSpeed > 5.0f && !bIsFalling;
}

void UClimbingAnimInstance::GetIsClimbing()
{
	bIsClimbing = CustomCharacterMovementComponent->IsClimbing();
}

void UClimbingAnimInstance::GetClimbVelocity()
{
	ClimbVelocity = CustomCharacterMovementComponent->GetUnrotatedClimbVelocity();
}
