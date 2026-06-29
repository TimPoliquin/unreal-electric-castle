// Copyright Alien Shores


#include "AI/Engagement/AIEngagementController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "AI/Engagement/Config/EngagementLevelConfig.h"
#include "AI/Engagement/Movement/EngagementMovementPlugin.h"
#include "AI/Perception/AIPerceptionActor.h"
#include "AI/Targeting/AITargetingActorInterface.h"
#include "AI/Targeting/AITargetingComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Perception/AIPerceptionComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "Utils/ArrayUtils.h"

UAIEngagementController::UAIEngagementController()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	BlockMovementTags = FGameplayTagContainer();
	BlockMovementTags.AddTagFast(GameplayTags.Effect_Block_AI);
	BlockMovementTags.AddTagFast(GameplayTags.Effect_Block_Movement);
}

void UAIEngagementController::BeginPlay()
{
	Super::BeginPlay();
	if (EngagementControlMode == EEngagementControlMode::Randomized)
	{
		RandomizeEngagement();
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &UAIEngagementController::HandleOwnerDeath);
	}
	if (const UEngagementLevelConfig* EngagementLevelConfig = GetEngagementLevelConfigByEngagementLevel(CurrentEngagementLevel))
	{
		SetMovementPlugin(EngagementLevelConfig->MovementPlugin);
	}
	SetComponentTickEnabled(bDrawVisualDebug);
}

void UAIEngagementController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bDrawVisualDebug)
	{
		const FString EngagementString = FString::Printf(TEXT("%s - %s"), *EnumName(CurrentEngagementLevel), *EnumName(CurrentEngagementRange));
		DrawDebugString(GetWorld(), GetOwner()->GetActorLocation() + FVector::UpVector * 100, EngagementString, nullptr, FColor::White, 0, true);
	}
}

void UAIEngagementController::Activate(const bool bReset)
{
	Super::Activate(bReset);
	SetComponentTickEnabled(bDrawVisualDebug);
	if (UEngagementLevelConfig* EngagementLevelConfig = GetEngagementLevelConfigByEngagementLevel(CurrentEngagementLevel))
	{
		SetMovementPlugin(EngagementLevelConfig->MovementPlugin);
	}
	if (EngagementControlMode == EEngagementControlMode::Randomized)
	{
		RandomizeEngagement();
	}
}

void UAIEngagementController::Deactivate()
{
	Super::Deactivate();
	SetComponentTickEnabled(false);
	if (GetCurrentEngagementControlMode() != EEngagementControlMode::Static)
	{
		CurrentEngagementLevel = EEngagementLevel::None;
		CurrentEngagementRange = EEngagementRange::None;
	}
	CurrentMovementPlugin = nullptr;
}

void UAIEngagementController::ChangeItUp_Implementation()
{
	if (CurrentMovementPlugin)
	{
		CurrentMovementPlugin->ChangeItUp();
	}
}

void UAIEngagementController::TickMovement(const float DeltaTime) const
{
	if (CurrentMovementPlugin && ShouldUpdateMovement())
	{
		CurrentMovementPlugin->TickMovement_Implementation(DeltaTime);
	}
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
	if (const UEngagementLevelConfig* EngagementLevelConfig = GetEngagementLevelConfigByEngagementLevel(CurrentEngagementLevel))
	{
		return EngagementLevelConfig->AttackRate;
	}
	return FFloatRange::Empty();
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
	FFloatRange AttackRate = FFloatRange::Empty();
	if (const UEngagementLevelConfig* EngagementLevelConfig = GetEngagementLevelConfigByEngagementLevel(CurrentEngagementLevel))
	{
		SetMovementPlugin(EngagementLevelConfig->MovementPlugin);
		AttackRate = EngagementLevelConfig->AttackRate;
	}
	OnEngagementLevelChanged.Broadcast({CurrentEngagementLevel, AttackRate});
}

void UAIEngagementController::SetCurrentEngagementRange(const EEngagementRange InEngagementRange)
{
	CurrentEngagementRange = InEngagementRange;
	if (CurrentMovementPlugin)
	{
		CurrentMovementPlugin->SetPreferredDistance(GetCurrentPreferredRange());
	}
	OnEngagementRangeChanged.Broadcast({CurrentEngagementRange, GetEngagementRangeConfigByEngagementRange(CurrentEngagementRange).PreferredDistance});
}

void UAIEngagementController::SetCurrentEngagementAbilityModes(const TArray<EEngagementAbilityMode> InEngagementAbilityModes)
{
	CurrentEngagementAbilityModes = InEngagementAbilityModes;
}

void UAIEngagementController::RandomizeEngagement()
{
	if (const UEngagementLevelConfig* EngagementLevelConfig = UArrayUtils::GetRandomElement(EngagementLevelConfigs))
	{
		SetCurrentEngagementLevel(EngagementLevelConfig->EngagementLevel);
	}
	SetCurrentEngagementRange(UArrayUtils::GetRandomElement(EngagementRangeConfigs).Range);
	if (CurrentMovementPlugin)
	{
		CurrentMovementPlugin->ChangeItUp();
	}
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
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Display, TEXT("[%s::%s] No potential targets actively perceived - attempting simple radius check"), *GetOwner()->GetName(), *GetName());
		}
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
	if (UAITargetingComponent* TargetingComponent = IAITargetingActorInterface::GetAITargetingComponent(GetOwner()))
	{
		TargetingComponent->SetCurrentTarget(NewTarget);
	}
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

bool UAIEngagementController::DoesPreferAggressiveEngagement() const
{
	return EngagementLevelConfigs.ContainsByPredicate(
		[](const UEngagementLevelConfig* EngagementLevelConfig)
		{
			return EngagementLevelConfig->EngagementLevel >= EEngagementLevel::Aggressive && EngagementLevelConfig->bIsPreferred;
		}
	);
}

EEngagementRange UAIEngagementController::GetPreferredEngagementRange() const
{
	const TArray<FEngagementRangeConfig> PreferredEngagementRangeConfigs = EngagementRangeConfigs.FilterByPredicate(
		[](const FEngagementRangeConfig& EngagementRangeConfig)
		{
			return EngagementRangeConfig.bIsPreferred;
		}
	);
	if (PreferredEngagementRangeConfigs.Num() > 0)
	{
		return UArrayUtils::GetRandomElement(PreferredEngagementRangeConfigs).Range;
	}
	return EEngagementRange::None;
}

EEngagementRange UAIEngagementController::GetRandomEngagementRange() const
{
	if (EngagementRangeConfigs.Num() > 0)
	{
		return UArrayUtils::GetRandomElement(EngagementRangeConfigs).Range;
	}
	return EEngagementRange::None;
}

EEngagementLevel UAIEngagementController::GetRandomAggressiveEngagementLevel() const
{
	if (EngagementLevelConfigs.Num() <= 0)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No engagement levels configured!"), *GetOwner()->GetName(), *GetName());
		return EEngagementLevel::None;
	}
	const TArray<UEngagementLevelConfig*> AggressiveConfigs = EngagementLevelConfigs.FilterByPredicate(
		[](const UEngagementLevelConfig* EngagementLevelConfig)
		{
			return IsValid(EngagementLevelConfig) && EngagementLevelConfig->EngagementLevel >= EEngagementLevel::Aggressive;
		}
	);
	if (AggressiveConfigs.Num() > 0)
	{
		return UArrayUtils::GetRandomElement(AggressiveConfigs)->EngagementLevel;
	}
	// in the case there are no aggressive engagement levels, just pick a random one
	return UArrayUtils::GetRandomElement(EngagementLevelConfigs)->EngagementLevel;
}

EEngagementLevel UAIEngagementController::GetRandomPassiveEngagementLevel(const bool bAllowNoneEngagement) const
{
	if (EngagementLevelConfigs.Num() <= 0)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No engagement levels configured!"), *GetOwner()->GetName(), *GetName());
		return EEngagementLevel::None;
	}
	const TArray<UEngagementLevelConfig*> PassiveConfigs = EngagementLevelConfigs.FilterByPredicate(
		[bAllowNoneEngagement](const UEngagementLevelConfig* EngagementLevelConfig)
		{
			if (bAllowNoneEngagement)
			{
				return EngagementLevelConfig->EngagementLevel <= EEngagementLevel::Cautious;
			}
			return EngagementLevelConfig->EngagementLevel <= EEngagementLevel::Cautious && EngagementLevelConfig->EngagementLevel > EEngagementLevel::None;
		}
	);
	if (PassiveConfigs.Num() > 0)
	{
		return UArrayUtils::GetRandomElement(PassiveConfigs)->EngagementLevel;
	}
	// in the case there are no passive engagement levels, just pick a random one
	return UArrayUtils::GetRandomElement(EngagementLevelConfigs)->EngagementLevel;
}

UEngagementLevelConfig* UAIEngagementController::GetEngagementLevelConfigByEngagementLevel(const EEngagementLevel InEngagementLevel) const
{
	for (UEngagementLevelConfig* Config : EngagementLevelConfigs)
	{
		if (IsValid(Config) && Config->EngagementLevel == InEngagementLevel)
		{
			return Config;
		}
	}
	return nullptr;
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

bool UAIEngagementController::ShouldUpdateMovement() const
{
	if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		return !AbilitySystemComponent->HasAnyMatchingGameplayTags(BlockMovementTags);
	}
	return true;
}

void UAIEngagementController::HandleOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}

void UAIEngagementController::SetMovementPlugin(UEngagementMovementPlugin* InEngagementMovementPlugin)
{
	if (IsValid(CurrentMovementPlugin))
	{
		CurrentMovementPlugin->RemoveDependencies(GetOwner());
	}
	CurrentMovementPlugin = InEngagementMovementPlugin;
	if (InEngagementMovementPlugin)
	{
		InEngagementMovementPlugin->InitializeDependencies(GetOwner());
		InEngagementMovementPlugin->SetPreferredDistance(GetCurrentPreferredRange());
	}
}
