// Copyright Alien Shores


#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Effect/CooldownGameplayEffect.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ElectricCastlePlayerController.h"


UElectricCastleGameplayAbility::UElectricCastleGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

FString UElectricCastleGameplayAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	return FString(TEXT(RICH_DEFAULT("Unimplemented")));
}

void UElectricCastleGameplayAbility::CommitExecute(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
)
{
	// Explicit override to remove cooldown application
	ApplyCost(Handle, ActorInfo, ActivationInfo);
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

void UElectricCastleGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const bool bReplicateEndAbility,
	const bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (bAutoApplyCooldownOnAbilityEnd && GetActorInfo().IsNetAuthority())
	{
		ApplyCustomCooldown();
	}
}

bool UElectricCastleGameplayAbility::ShouldSetMotionTarget() const
{
	if (const APawn* PlayerPawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		if (const AElectricCastlePlayerController* ElectricCastlePlayerController = Cast<
			AElectricCastlePlayerController>(PlayerPawn->GetController()))
		{
			return ElectricCastlePlayerController->IsInputTypeMouse() && ElectricCastlePlayerController->
				IsTargetingEnemy();
		}
	}
	return false;
}

void UElectricCastleGameplayAbility::GetActorDifference(
	const TArray<AActor*>& AList,
	const TSet<AActor*>& BList,
	TArray<AActor*>& OutDifference
)
{
	for (AActor* Actor : AList)
	{
		if (!BList.Contains(Actor))
		{
			OutDifference.Add(Actor);
		}
	}
}

void UElectricCastleGameplayAbility::DebugLog(
	const FString LogString,
	const float TimeToDisplay,
	const FColor Color
) const
{
	if (bDebug)
	{
		const FString DebugString = FString::Printf(
			TEXT("[%s][%s] %s"),
			GetAvatarActorFromActorInfo()->HasAuthority()
				? *FString("Server")
				: *FString("Client"),
			*GetName(),
			*LogString
		);
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, Color, DebugString);
	}
}

bool UElectricCastleGameplayAbility::GetComboMontageHitLocationFromList(
	const int32 ComboIdx,
	const TArray<FComboAbilityConfig> ComboConfigs,
	FVector& ComboHitLocation
) const
{
	if (!ComboConfigs.IsValidIndex(ComboIdx))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Invalid Combo Index: %d"), *GetName(), ComboIdx);
		return false;
	}
	return GetComboMontageHitLocation(ComboConfigs[ComboIdx], ComboHitLocation);
}

bool UElectricCastleGameplayAbility::GetComboMontageHitLocation(
	const FComboAbilityConfig& ComboConfig,
	FVector& ComboHitLocation
) const
{
	const USkeletalMeshComponent* MeshComponent = nullptr;
	if (ComboConfig.bIsWeaponAbility)
	{
		MeshComponent = ICombatInterface::GetWeapon(GetAvatarActorFromActorInfo());
	}
	else if (const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		MeshComponent = Character->GetMesh();
	}
	if (MeshComponent)
	{
		ComboHitLocation = MeshComponent->GetSocketLocation(ComboConfig.SocketName);
		return true;
	}
	return false;
}

FRotator UElectricCastleGameplayAbility::GetAvatarActorForwardRotator() const
{
	return UKismetMathLibrary::MakeRotFromX(GetAvatarActorFromActorInfo()->GetActorForwardVector());
}

FRotator UElectricCastleGameplayAbility::CalculateRotationToTarget(const AActor* Target, const FVector StartingLocation, const float ProjectileSpeed, const bool bAffectedByGravity) const
{
	if (!Target)
	{
		return GetAvatarActorForwardRotator();
	}
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartingLocation, Target->GetActorLocation());
	if (bAffectedByGravity)
	{
		if (float PitchOverride; UElectricCastleAbilitySystemLibrary::CalculatePitchToHitTarget(StartingLocation, Target->GetActorLocation(), ProjectileSpeed, PitchOverride))
		{
			Rotation.Pitch = PitchOverride;
			return Rotation;
		}
		return GetAvatarActorForwardRotator();
	}
	return Rotation;
}

void UElectricCastleGameplayAbility::FaceHitTarget_Implementation(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		ICombatInterface::UpdateFacingTarget(GetAvatarActorFromActorInfo(), HitResult.ImpactPoint);
	}
	else
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
	if (const UGameplayEffect* LocalCooldownEffect = GetCooldownGameplayEffect())
	{
		float Cooldown = 0.f;
		LocalCooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
		return Cooldown;
	}
	return 0.f;
}

void UElectricCastleGameplayAbility::ApplyCustomCooldown() const
{
	if (!CooldownConfig.IsValid())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] CooldownConfig is invalid."), *GetName());
		return;
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetAvatarActorFromActorInfo()
	))
	{
		// Build a GameplayEffectSpec on the fly
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
			UCooldownGameplayEffect::StaticClass(),
			// base GE class
			GetAbilityLevel(),
			// level
			ASC->MakeEffectContext() // context
		);

		if (SpecHandle.IsValid())
		{
			if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
			{
				// Set duration
				Spec->SetDuration(CooldownConfig.Duration.GetValueAtLevel(GetAbilityLevel()), true);
				// 1. Add as an asset tag (tag belongs to the GE itself)
				Spec->AddDynamicAssetTag(CooldownConfig.CooldownTag);
				// 2. Grant to the target actor (tag is applied to ASC while GE is active)
				Spec->DynamicGrantedTags.AddTag(CooldownConfig.CooldownTag);
				// Apply to self
				ASC->ApplyGameplayEffectSpecToSelf(*Spec);
			}
		}
	}
}
