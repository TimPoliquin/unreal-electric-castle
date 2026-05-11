// Copyright Alien Shores


#include "AI/Engagement/AIEngagementController.h"

#include "AI/Perception/AIPerceptionActor.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Perception/AIPerceptionComponent.h"
#include "Utils/ArrayUtils.h"

UAIEngagementController::UAIEngagementController()
{
	PrimaryComponentTick.bCanEverTick = false;
	EngagementLevelConfigs = {
		FEngagementLevelConfig(EEngagementLevel::None, FFloatRange::Empty()),
		FEngagementLevelConfig(EEngagementLevel::Passive, FFloatRange::Inclusive(60.f, 120.f)),
		FEngagementLevelConfig(EEngagementLevel::Cautious, FFloatRange::Inclusive(25.f, 45.f)),
		FEngagementLevelConfig(EEngagementLevel::Aggressive, FFloatRange::Inclusive(5.f, 15.f)),
		FEngagementLevelConfig(EEngagementLevel::AllOut, FFloatRange::Inclusive(0.f, 5.f))
	};
	EngagementRangeConfigs = {
		FEngagementRangeConfig(EEngagementRange::None, FFloatRange::Empty()),
		FEngagementRangeConfig(EEngagementRange::Close, FFloatRange::Inclusive(0.f, 250.f)),
		FEngagementRangeConfig(EEngagementRange::Ranged, FFloatRange::Inclusive(250.f, 600.f)),
		FEngagementRangeConfig(EEngagementRange::HangBack, FFloatRange::Inclusive(400.f, 1000.f))
	};
	EngagementAbilityModeConfigs = {
		FEngagementAbilityModeConfig(EEngagementAbilityMode::None),
		FEngagementAbilityModeConfig(EEngagementAbilityMode::Attack),
		FEngagementAbilityModeConfig(EEngagementAbilityMode::Defend),
		FEngagementAbilityModeConfig(EEngagementAbilityMode::Support)
	};
}

void UAIEngagementController::BeginPlay()
{
	Super::BeginPlay();
	if (EngagementControlMode == EEngagementControlMode::Randomized)
	{
		RandomizeEngagement();
	}
}

void UAIEngagementController::Activate(const bool bReset)
{
	Super::Activate(bReset);
	if (EngagementControlMode == EEngagementControlMode::Randomized)
	{
		RandomizeEngagement();
	}
}

void UAIEngagementController::Deactivate()
{
	Super::Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(RandomizedEngagementTimerHandle);
}

EEngagementControlMode UAIEngagementController::GetCurrentEngagementControlMode() const
{
	return EngagementControlMode;
}

EEngagementLevel UAIEngagementController::GetCurrentEngagementLevel() const
{
	return CurrentEngagementLevel;
}

FFloatRange UAIEngagementController::GetCurrentEngagementAttackRate() const
{
	return GetEngagementLevelConfigByEngagementLevel(CurrentEngagementLevel).AttackRate;
}

EEngagementRange UAIEngagementController::GetCurrentEngagementRange() const
{
	return CurrentEngagementRange;
}

FFloatRange UAIEngagementController::GetCurrentPreferredRange() const
{
	return GetEngagementRangeConfigByEngagementRange(CurrentEngagementRange).PreferredDistance;
}

void UAIEngagementController::SetCurrentEngagementControlMode(const EEngagementControlMode InEngagementControlMode)
{
	EngagementControlMode = InEngagementControlMode;
}

void UAIEngagementController::SetCurrentEngagementLevel(const EEngagementLevel InEngagementLevel)
{
	CurrentEngagementLevel = InEngagementLevel;
	OnEngagementLevelChanged.Broadcast({CurrentEngagementLevel, GetEngagementLevelConfigByEngagementLevel(CurrentEngagementLevel).AttackRate});
}

void UAIEngagementController::SetCurrentEngagementRange(const EEngagementRange InEngagementRange)
{
	CurrentEngagementRange = InEngagementRange;
	OnEngagementRangeChanged.Broadcast({CurrentEngagementRange, GetEngagementRangeConfigByEngagementRange(CurrentEngagementRange).PreferredDistance});
}

void UAIEngagementController::SetCurrentEngagementAbilityModes(const TArray<EEngagementAbilityMode> InEngagementAbilityModes)
{
	CurrentEngagementAbilityModes = InEngagementAbilityModes;
}

void UAIEngagementController::RandomizeEngagement()
{
	SetCurrentEngagementLevel(UArrayUtils::GetRandomElement(EngagementLevelConfigs).EngagementLevel);
	SetCurrentEngagementRange(UArrayUtils::GetRandomElement(EngagementRangeConfigs).Range);
	// Leave engagement ability modes alone - allow all ability modes to continue to operate for maximum randomness
	GetWorld()->GetTimerManager().SetTimer(
		RandomizedEngagementTimerHandle,
		[this]()
		{
			RandomizeEngagement();
			ChooseNewRandomTarget();
		},
		URandRangeBlueprintLibrary::GetRandomFloatInRange(RandomizeTime),
		false
	);
}

AActor* UAIEngagementController::ChooseNewRandomTarget()
{
	const TArray ActorsToIgnore = {GetOwner()};
	TArray<AActor*> PotentialTargets;
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Display, TEXT("[%s::%s] Choosing a new random target"), *GetOwner()->GetName(), *GetName());
	}
	if (const UAIPerceptionComponent* PerceptionComponent = IAIPerceptionActor::GetAIPerceptionComponent(GetOwner()))
	{
		// first, try to find targets using the actor's perception
		PerceptionComponent->GetCurrentlyPerceivedActors(nullptr, PotentialTargets);
	}
	if (PotentialTargets.IsEmpty())
	{
		UE_LOG(LogElectricCastle, Display, TEXT("[%s::%s] No potential targets actively perceived - attempting simple radius check"), *GetOwner()->GetName(), *GetName());
		// if no targets are perceived, try to find actors within the radius
		UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinRadius(
			GetOwner(),
			ActorsToIgnore,
			ICombatInterface::GetTargetTagsToIgnore(GetOwner()),
			GetOwner()->GetActorLocation(),
			1000.f,
			PotentialTargets,
			bDebug
		);
	}
	AActor* NewTarget = PotentialTargets.Num() > 0 ? UArrayUtils::GetRandomElement(PotentialTargets) : nullptr;
	SetEngagementTarget(NewTarget);
	return NewTarget;
}

FGameplayTag UAIEngagementController::ChooseAbility() const
{
	TArray<FGameplayTag> AbilityTags;
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (
				UElectricCastleAbilitySystemLibrary::IsAbilitySupportedInEngagementRange(Spec, CurrentEngagementRange) &&
				UElectricCastleAbilitySystemLibrary::IsAbilitySupportedInEngagementModes(Spec, CurrentEngagementAbilityModes))
			{
				AbilityTags.Add(UElectricCastleAbilitySystemLibrary::GetAbilityTagFromSpec(Spec));
			}
		}
	}
	if (AbilityTags.Num() > 0)
	{
		return UArrayUtils::GetRandomElement(AbilityTags);
	}
	return FGameplayTag::EmptyTag;
}

void UAIEngagementController::SetEngagementTarget(AActor* InTarget)
{
	if (bDebug)
	{
		if (IsValid(InTarget))
		{
			UE_LOG(LogElectricCastle, Display, TEXT("[%s::%s] New target set: %s"), *GetOwner()->GetName(), *GetName(), *InTarget->GetName());
		}
		else
		{
			UE_LOG(LogElectricCastle, Display, TEXT("[%s::%s] Target cleared"), *GetOwner()->GetName(), *GetName());
		}
	}
	Target = InTarget;
	OnEngagementTargetChanged.Broadcast(Target.Get());
}

FEngagementLevelConfig UAIEngagementController::GetEngagementLevelConfigByEngagementLevel(const EEngagementLevel InEngagementLevel) const
{
	for (const FEngagementLevelConfig& Config : EngagementLevelConfigs)
	{
		if (Config.EngagementLevel == InEngagementLevel)
		{
			return Config;
		}
	}
	return FEngagementLevelConfig(EEngagementLevel::None, FFloatRange::Empty());
}

FEngagementRangeConfig UAIEngagementController::GetEngagementRangeConfigByEngagementRange(const EEngagementRange InEngagementRange) const
{
	for (const FEngagementRangeConfig& Config : EngagementRangeConfigs)
	{
		if (Config.Range == InEngagementRange)
		{
			return Config;
		}
	}
	return FEngagementRangeConfig(EEngagementRange::None, FFloatRange::Empty());
}
