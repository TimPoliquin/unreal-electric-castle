// Copyright Alien Shores


#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Interaction/CombatInterface.h"
#include "Player/ElectricCastlePlayerController.h"

FString UElectricCastleGameplayAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	return FString(TEXT(RICH_DEFAULT("Unimplemented")));
}

void UElectricCastleGameplayAbility::ExecuteTask(UAbilityTask* Task) const
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetActorInfo().AbilitySystemComponent.Get())
	{
		if (Task)
		{
			Task->SetAbilitySystemComponent(AbilitySystemComponent);
			Task->ReadyForActivation();
		}
	}
}

bool UElectricCastleGameplayAbility::ShouldSetMotionTarget() const
{
	if (const APawn* PlayerPawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		if (const AElectricCastlePlayerController* ElectricCastlePlayerController = Cast<AElectricCastlePlayerController>(PlayerPawn->GetController()))
		{
			return ElectricCastlePlayerController->IsInputTypeMouse() && ElectricCastlePlayerController->IsTargetingEnemy();
		}
	}
	return false;
}

void UElectricCastleGameplayAbility::FaceHitTarget_Implementation(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		ICombatInterface::UpdateFacingTarget(GetAvatarActorFromActorInfo(), HitResult.ImpactPoint);
	} else
	{
		ICombatInterface::ClearFacingTarget(GetAvatarActorFromActorInfo());
	}
}

float UElectricCastleGameplayAbility::GetManaCost(const float InLevel) const
{
	if (GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo Mod : GetCostGameplayEffect()->Modifiers)
		{
			if (Mod.Attribute == UElectricCastleAttributeSet::GetManaAttribute())
			{
				float ManaCost = 0.f;
				// TODO this assumes a static value.
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				return ManaCost;
			}
		}
	}
	return 0.f;
}

float UElectricCastleGameplayAbility::GetCooldown(const float InLevel) const
{
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		float Cooldown = 0.f;
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
		return Cooldown;
	}
	return 0.f;
}
