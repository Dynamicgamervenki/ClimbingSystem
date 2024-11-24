// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ClimbingSystem/ClimbingSystemCharacter.h"

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TraceClimableSurfaces();
	TraceFromEyeHeight(100.0f);
}

TArray<FHitResult> UCustomCharacterMovementComponent::DoCapsuleTraceMultiForObjects(const FVector& Start,
                                                                                    const FVector& End, bool bShowDebugShape)
{
	TArray<FHitResult> CapsuleTraceHitResults;
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
		bShowDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		CapsuleTraceHitResults,
		false
	);
	return CapsuleTraceHitResults;
}

FHitResult UCustomCharacterMovementComponent::DoLineTraceSingleForObject(const FVector& Start, const FVector& End,
	bool bShowDebugShape)
{
	FHitResult LineHitResult;

	UKismetSystemLibrary::LineTraceSingleForObjects
	(
		GetWorld(),
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(), 
		bShowDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		LineHitResult,
		false
	);
	
	return LineHitResult;
}


void UCustomCharacterMovementComponent::TraceClimableSurfaces()
{
	FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.0f;
	FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	FVector End = Start + UpdatedComponent->GetForwardVector();
	DoCapsuleTraceMultiForObjects(Start, End, true);
}

void UCustomCharacterMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	FVector EyeOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
	const FVector Start = ComponentLocation + EyeOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	DoLineTraceSingleForObject(Start, End, true);
}
