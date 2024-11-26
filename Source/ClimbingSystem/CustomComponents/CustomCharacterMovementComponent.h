// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName = "Climb Mode")
	};
}

/**
 * 
 */
UCLASS()
class CLIMBINGSYSTEM_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected: 
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxSpeed() const override;
	
	private:

	UFUNCTION()
	TArray<FHitResult> DoCapsuleTraceMultiForObjects(const FVector& Start,const FVector& End,bool bShowDebugShape = false,bool bDrawPersistantShapes = false);
	UFUNCTION()
	FHitResult DoLineTraceSingleForObject(const FVector& Start,const FVector& End,bool bShowDebugShape = false,bool bDrawPersistantShapes = false);
	
	UFUNCTION()
	
	bool TraceClimableSurfaces();
	UFUNCTION()
	
	FHitResult TraceFromEyeHeight(float TraceDistance,float TraceStartOffset = 0.f);
	//Trace Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ClimbableSurfaceTraceTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float ClimbCapsuleRadius = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float ClimbCapsuleHalfHeight = 72.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float MaxBreakClimbDeceleration = 400.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float MaxClimbSpeed = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float MaxClimbAccelration = 300.0f;

	TArray<FHitResult> ClimbableSurfaceSTracedResults;

	FVector CurrentClimbableSurfaceLocation;
	FVector CurrentClimbableSurfaceNormal;
public:
	
	UFUNCTION()
	void ToogleClimbing(bool bEnableClimb);
	UFUNCTION()
	bool IsClimbing() const;
	UFUNCTION()
	bool CanStartClimbing();
	UFUNCTION()
	void StartClimbing();
	UFUNCTION()
	void StopClimbing();
	UFUNCTION()
	void PhysClimb(float deltaTime, int32 Iterations);
	UFUNCTION()
	void ProcessClimbableSurfaceInfo();
	UFUNCTION()
	FQuat GetClimbRotation(float DeltaTime);
	UFUNCTION()
	void SnapMovemnetToClimbableSurfaces(float DeltaTime);
};
