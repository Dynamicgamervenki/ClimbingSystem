// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

#include "AI/NavigationSystemBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ClimbingSystem/ClimbingSystemCharacter.h"
#include "Components/CapsuleComponent.h"
#include "ClimbingSystem/DebugHelpers.h"

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// TraceClimableSurfaces();
	// TraceFromEyeHeight(100.0f);
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.0f);
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(92.0f);
		
		StopMovementImmediately();
	}
	
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (IsClimbing())
	{
		PhysClimb(deltaTime,Iterations);
	}
	Super::PhysCustom(deltaTime, Iterations);
}

TArray<FHitResult> UCustomCharacterMovementComponent::DoCapsuleTraceMultiForObjects(const FVector& Start,
                                                                                    const FVector& End, bool bShowDebugShape,bool bDrawPersistantShapes)
{
	TArray<FHitResult> CapsuleTraceHitResults;
	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;

	if (bShowDebugShape)
	{
		DebugTrace = EDrawDebugTrace::ForOneFrame;
		if (bDrawPersistantShapes)
		{
			DebugTrace = EDrawDebugTrace::Persistent;
		}
	}
	UKismetSystemLibrary::CapsuleTraceMultiForObjects
	(
		GetWorld(),
		Start,
		End,
		ClimbCapsuleRadius,
		ClimbCapsuleHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(), 
		DebugTrace,
		CapsuleTraceHitResults,
		false
	);
	return CapsuleTraceHitResults;
}

FHitResult UCustomCharacterMovementComponent::DoLineTraceSingleForObject(const FVector& Start, const FVector& End,
	bool bShowDebugShape,bool bDrawPersistantShapes)
{
	FHitResult LineHitResult;

	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;

	if (bShowDebugShape)
	{
		DebugTrace = EDrawDebugTrace::ForOneFrame;
		if (bDrawPersistantShapes)
		{
			DebugTrace = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::LineTraceSingleForObjects
	(
		GetWorld(),
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(), 
		DebugTrace,
		LineHitResult,
		false
	);
	
	return LineHitResult;
}


bool UCustomCharacterMovementComponent::TraceClimableSurfaces()
{
	FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.0f;
	FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	FVector End = Start + UpdatedComponent->GetForwardVector();
	ClimbableSurfaceSTracedResults = DoCapsuleTraceMultiForObjects(Start, End, true);

	return !ClimbableSurfaceSTracedResults.IsEmpty();
}

FHitResult UCustomCharacterMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	FVector EyeOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
	const FVector Start = ComponentLocation + EyeOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleForObject(Start, End, true);
}

void UCustomCharacterMovementComponent::ToogleClimbing(bool bEnableClimb)
{
	if (bEnableClimb)
	{
		if (CanStartClimbing())
		{
			//start climbing
			StartClimbing();
		}
		else
		{
			Debug::Print(TEXT("Can Not Climb"));
		}
	}
	else
	{
		//stop climbing
		StopClimbing();
	}
}

bool UCustomCharacterMovementComponent::IsClimbing() const
{
	 return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
}

bool UCustomCharacterMovementComponent::CanStartClimbing()
{
	if (IsFalling()) return false;
	if (!TraceClimableSurfaces()) return false;
	if (!TraceFromEyeHeight(100.0f).bBlockingHit) return false;

	return true;
}

void UCustomCharacterMovementComponent::StartClimbing()
{
	SetMovementMode(MOVE_Custom,ECustomMovementMode::MOVE_Climb);
}

void UCustomCharacterMovementComponent::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

void UCustomCharacterMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	/* Process All Climbable Surface Info */
	TraceClimableSurfaces();
	ProcessClimbableSurfaceInfo();

	if (CheckShouldStopClimbing())
	{
		StopClimbing();
	}
	
	/* Check If We Should Stop Clibing */
	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		//Define Max ClimbSpeed And Accelration
		CalcVelocity(deltaTime, 0.0f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);

	//Handle Climb Rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	/* Snap Movement To Climbable Surfaces */
	SnapMovemnetToClimbableSurfaces(deltaTime);
}

void UCustomCharacterMovementComponent::ProcessClimbableSurfaceInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	if(ClimbableSurfaceSTracedResults.IsEmpty()) return;

	for (const FHitResult& TracedHitResult : ClimbableSurfaceSTracedResults)
	{
		CurrentClimbableSurfaceLocation += TracedHitResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += TracedHitResult.ImpactNormal;
	}

	CurrentClimbableSurfaceLocation /= ClimbableSurfaceSTracedResults.Num();
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();

	// Debug::Print(TEXT("ClimbableSurfaceLocation") + CurrentClimbableSurfaceLocation.ToCompactString(),FColor::Black,1);
	// Debug::Print(TEXT("ClimbableSurfaceNormal") + CurrentClimbableSurfaceNormal.ToCompactString(),FColor::Black,2);
}

FQuat UCustomCharacterMovementComponent::GetClimbRotation(float DeltaTime)
{
	const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}

	const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();

	return FMath::QInterpTo(CurrentQuat,TargetQuat,DeltaTime,5.0f);
	
}

void UCustomCharacterMovementComponent::SnapMovemnetToClimbableSurfaces(float DeltaTime)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();

	const FVector ProjectedCharacterToSurface = (CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);

	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();

	UpdatedComponent->MoveComponent(SnapVector * DeltaTime * MaxClimbSpeed,UpdatedComponent->GetComponentQuat(),true);
}

bool UCustomCharacterMovementComponent::CheckShouldStopClimbing()
{
	if (ClimbableSurfaceSTracedResults.IsEmpty()) return true;

	const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal,FVector::UpVector);
	const float DegreeDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	Debug::Print(TEXT("Degree Diff") + FString::SanitizeFloat(DegreeDiff),FColor::Black,2);

	if (DegreeDiff <= 60.0f)
	{
		return true;
	}
	
	return false;
}

float UCustomCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsClimbing())
	{
		return 	MaxClimbAccelration;
	}
	else
	{
		return Super::GetMaxAcceleration();
	}
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsClimbing())
	{
		return MaxClimbSpeed;
	}
	else
	{
		return Super::GetMaxSpeed();
	}
}

