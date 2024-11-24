// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CLIMBINGSYSTEM_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public :
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	private:

	UFUNCTION()
	TArray<FHitResult> DoCapsuleTraceMultiForObjects(const FVector& Start,const FVector& End,bool bShowDebugShape = false);
	UFUNCTION()
	FHitResult DoLineTraceSingleForObject(const FVector& Start,const FVector& End,bool bShowDebugShape = false);
	
	UFUNCTION()
	void TraceClimableSurfaces();
	UFUNCTION()
	void TraceFromEyeHeight(float TraceDistance,float TraceStartOffset = 0.f);
	//Trace Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ClimbableSurfaceTraceTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float ClimbCapsuleRadius = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float ClimbCapsuleHalfHeight = 72.0f;
	
};
