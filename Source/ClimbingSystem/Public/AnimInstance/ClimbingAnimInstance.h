// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ClimbingAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLIMBINGSYSTEM_API UClimbingAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public : 
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private :
	UPROPERTY()
	class AClimbingSystemCharacter* ClimbingSystemCharacter;
	UPROPERTY()
	class UCustomCharacterMovementComponent* CustomCharacterMovementComponent;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	float AirSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	bool bShouldMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	bool bIsClimbing;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	FVector ClimbVelocity;
	

public :
		void GetGroundSpeed();
		void GetIsFalling();
		void GetAirSpeed();
		void GetShouldMove();
		void GetIsClimbing();
	void GetClimbVelocity();	
};
