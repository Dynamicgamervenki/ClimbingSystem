// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ClimbingSystem/ClimbingSystemCharacter.h"
#include "ClimbingSystem/DebugHelpers.h"

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// TraceClimableSurfaces();
	// TraceFromEyeHeight(100.0f);
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
	ClimbableSurfaceSTracedResults = DoCapsuleTraceMultiForObjects(Start, End, true,true);

	return !ClimbableSurfaceSTracedResults.IsEmpty();
}

FHitResult UCustomCharacterMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	FVector EyeOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
	const FVector Start = ComponentLocation + EyeOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleForObject(Start, End, true,true);
}

void UCustomCharacterMovementComponent::ToogleClimbing(bool bEnableClimb)
{
	if (bEnableClimb)
	{
		if (CanStartClimbing())
		{
			//start climbing
			Debug::Print(TEXT("Can Climb"));
		}
		else
		{
			Debug::Print(TEXT("Can Not Climb"));
		}
	}
	else
	{
		//stop climbing
	}
}

bool UCustomCharacterMovementComponent::IsClimbing()
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

