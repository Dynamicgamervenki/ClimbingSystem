// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClimbingSystemCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CustomComponents/CustomCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DebugHelpers.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AClimbingSystemCharacter

AClimbingSystemCharacter::AClimbingSystemCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CustomMovementComponent = Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate


	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
}

void AClimbingSystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AClimbingSystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClimbingSystemCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AClimbingSystemCharacter::Look);
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &AClimbingSystemCharacter::ClimbActionStarted);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AClimbingSystemCharacter::Move(const FInputActionValue& Value)
{
	if (!CustomMovementComponent) return;

	if (CustomMovementComponent->IsClimbing())
	{
		HandleClimbMovementInput(Value);
	}
	else
	{
		HandleGroundMovementInput(Value);
	}
}

void AClimbingSystemCharacter::HandleGroundMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AClimbingSystemCharacter::HandleClimbMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FVector ForwardDirection = FVector::CrossProduct(-CustomMovementComponent->GetClimbableSurfaceNormal(),GetActorRightVector());
	const FVector RightDirection = FVector::CrossProduct(-CustomMovementComponent->GetClimbableSurfaceNormal(),-GetActorUpVector());

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AClimbingSystemCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AClimbingSystemCharacter::ClimbActionStarted(const FInputActionValue& Value)
{
	if (!CustomMovementComponent) return;
	if (!CustomMovementComponent->IsClimbing())
	{
		CustomMovementComponent->ToogleClimbing(true);
	}
	else
	{
		CustomMovementComponent->ToogleClimbing(false);
	}
}
