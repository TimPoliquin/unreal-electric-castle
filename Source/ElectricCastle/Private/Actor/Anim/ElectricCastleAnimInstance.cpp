// Copyright Alien Shores


#include "Actor/Anim/ElectricCastleAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "Tags/ElectricCastleGameplayTags.h"

void UElectricCastleAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateVelocity();
	UpdateRotation(DeltaSeconds);
	UpdateIsFalling();
	UpdateIsDead();
	UpdateIsAiming();
}

ACharacter* UElectricCastleAnimInstance::TryGetCharacterOwner()
{
	if (!CharacterOwner)
	{
		CharacterOwner = Cast<ACharacter>(TryGetPawnOwner());
	}
	return CharacterOwner;
}

UCharacterMovementComponent* UElectricCastleAnimInstance::TryGetCharacterMovementComponent()
{
	if (!CharacterMovementComponent)
	{
		if (const ACharacter* Character = TryGetCharacterOwner())
		{
			CharacterMovementComponent = Character->GetCharacterMovement();
		}
	}
	return CharacterMovementComponent;
}

AController* UElectricCastleAnimInstance::TryGetController()
{
	if (!CharacterController)
	{
		if (const ACharacter* Character = TryGetCharacterOwner())
		{
			CharacterController = Character->GetController();
		}
	}
	return CharacterController;
}

void UElectricCastleAnimInstance::UpdateIsAiming_Implementation()
{
	if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner()))
	{
		bIsAiming = AbilitySystemComponent->HasMatchingGameplayTag(FElectricCastleGameplayTags::Get().Effect_State_Aiming);
	}
	else
	{
		bIsAiming = false;
	}
}

void UElectricCastleAnimInstance::UpdateIsDead_Implementation()
{
	bIsDead = ICombatInterface::IsDead(TryGetPawnOwner());
}

void UElectricCastleAnimInstance::UpdateIsFalling_Implementation()
{
	if (const UCharacterMovementComponent* MovementComponent = TryGetCharacterMovementComponent())
	{
		bIsFalling = MovementComponent->IsFalling();
	}
}

void UElectricCastleAnimInstance::UpdateRotation_Implementation(const float DeltaSeconds)
{
	const ACharacter* Character = TryGetCharacterOwner();
	const AController* Controller = TryGetController();
	if (Character && Controller)
	{
		bIsTurning = Character->bUseControllerRotationYaw;
		// Get current controller yaw
		const float CurrentControllerYaw = Controller->GetControlRotation().Yaw;
		// Calculate how fast the controller is rotating
		const float YawDelta = FMath::FindDeltaAngleDegrees(PreviousYaw, CurrentControllerYaw);
		const float ControllerYawSpeed = YawDelta / DeltaSeconds; // Degrees per second

		// Smooth it for blend space
		SmoothedYawDelta = FMath::FInterpTo(
			SmoothedYawDelta,
			ControllerYawSpeed,
			DeltaSeconds,
			6.0f // Adjust smoothing speed as needed
		);
		// Store for next frame
		PreviousYaw = CurrentControllerYaw;
	}
	else
	{
		bIsTurning = false;
		SmoothedYawDelta = 0.f;
	}
}

void UElectricCastleAnimInstance::UpdateVelocity_Implementation()
{
	if (const UCharacterMovementComponent* MovementComponent = TryGetCharacterMovementComponent())
	{
		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Length();
	}
	else
	{
		Velocity = FVector::ZeroVector;
		GroundSpeed = 0.f;
	}
	bIsMoving = GroundSpeed > 0.f;
}
