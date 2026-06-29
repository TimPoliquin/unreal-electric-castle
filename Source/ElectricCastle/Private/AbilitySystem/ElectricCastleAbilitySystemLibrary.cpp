// Copyright Alien Shores


#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "KismetTraceUtils.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Effect/DurationGameplayEffect.h"
#include "AbilitySystem/Effect/InfiniteGameplayEffect.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Character/ElectricCastleCharacter.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/HUD/ElectricCastleHUD.h"

UAttributeMenuWidgetController* UElectricCastleAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject
)
{
	if (const AElectricCastleHUD* HUD = GetElectricCastleHUD(WorldContextObject))
	{
		return HUD->GetAttributeMenuWidgetController();
	}
	return nullptr;
}

USpellMenuWidgetController* UElectricCastleAbilitySystemLibrary::GetSpellMenuWidgetController(
	const UObject* WorldContextObject
)
{
	if (const AElectricCastleHUD* HUD = GetElectricCastleHUD(WorldContextObject))
	{
		return HUD->GetSpellMenuWidgetController();
	}
	return nullptr;
}

UElectricCastleAbilitySystemComponent* UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(AActor* Actor)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		Actor
	))
	{
		return Cast<UElectricCastleAbilitySystemComponent>(AbilitySystemComponent);
	}
	return nullptr;
}

UCharacterClassInfo* UElectricCastleAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if (const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(
		WorldContextObject
	))
	{
		return GameDataSubsystem->GetCharacterClassInfo();
	}
	return nullptr;
}

UAbilityInfo* UElectricCastleAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	if (const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(
		WorldContextObject
	))
	{
		return GameDataSubsystem->GetAbilityInfo();
	}
	return nullptr;
}

UElectricCastleAttributeSet* UElectricCastleAbilitySystemLibrary::GetAttributeSet(const UObject* Actor)
{
	if (const AElectricCastleCharacter* Character = Cast<AElectricCastleCharacter>(Actor))
	{
		return Character->GetAttributeSet();
	}
	return nullptr;
}

bool UElectricCastleAbilitySystemLibrary::IsFullHealth(AActor* Actor)
{
	if (const AElectricCastleCharacter* Character = Cast<AElectricCastleCharacter>(Actor))
	{
		if (Character->GetAttributeSet())
		{
			return Character->GetAttributeSet()->IsFullHealth();
		}
	}
	return false;
}

bool UElectricCastleAbilitySystemLibrary::IsFullMana(AActor* Actor)
{
	if (const AElectricCastleCharacter* Character = Cast<AElectricCastleCharacter>(Actor))
	{
		if (Character->GetAttributeSet())
		{
			return Character->GetAttributeSet()->IsFullMana();
		}
	}
	return false;
}

float UElectricCastleAbilitySystemLibrary::GetLungeDistance(const AActor* Owner)
{
	if (const UElectricCastleAttributeSet* AttributeSet = GetAttributeSet(Owner))
	{
		return AttributeSet->GetLungeDistance();
	}
	return 0.f;
}

float UElectricCastleAbilitySystemLibrary::GetVisibilityAttributeValue(const AActor* Owner)
{
	if (const UElectricCastleAttributeSet* AttributeSet = GetAttributeSet(Owner))
	{
		return AttributeSet->GetVisibility();
	}
	return 1.f;
}

bool UElectricCastleAbilitySystemLibrary::IsInfiniteEffect(const FGameplayEffectSpecHandle& SpecHandle)
{
	return SpecHandle.Data.Get()->Def.Get()->DurationPolicy ==
		EGameplayEffectDurationType::Infinite;
}

bool UElectricCastleAbilitySystemLibrary::IsInstantEffect(const FGameplayEffectSpecHandle& SpecHandle)
{
	return SpecHandle.Data.Get()->Def.Get()->DurationPolicy ==
		EGameplayEffectDurationType::Instant;
}

void UElectricCastleAbilitySystemLibrary::CopyEffectContextHandleProperties(const FGameplayEffectContextHandle& Source, FGameplayEffectContextHandle& Target)
{
	SetIsBlockedHit(Target, IsBlockedHit(Source));
	SetIsCriticalHit(Target, IsCriticalHit(Source));
	SetIsEvadedAttack(Target, IsEvadedAttack(Source));
	SetIsParriedAttack(Target, IsParriedAttack(Source));
	SetDamageTypeTag(Target, GetDamageTypeTag(Source));
	SetDeathImpulse(Target, GetDeathImpulse(Source));
	SetDebuffTypeTag(Target, GetDebuffTypeTag(Source));
	SetDebuffDamage(Target, GetDebuffDamage(Source));
	SetDebuffDuration(Target, GetDebuffDuration(Source));
	SetDebuffFrequency(Target, GetDebuffFrequency(Source));
	SetIsSuccessfulDebuff(Target, IsSuccessfulDebuff(Source));
	SetIsRadialDamage(Target, IsRadialDamage(Source));
	SetKnockbackVector(Target, GetKnockbackVector(Source));
	SetRadialDamageInnerRadius(Target, GetRadialDamageInnerRadius(Source));
	SetRadialDamageOrigin(Target, GetRadialDamageOrigin(Source));
	SetRadialDamageOuterRadius(Target, GetRadialDamageOuterRadius(Source));
}

int UElectricCastleAbilitySystemLibrary::GetCharacterLevel(UAbilitySystemComponent* AbilitySystemComponent)
{
	return IElectricCastleAbilitySystemInterface::GetCharacterLevel(AbilitySystemComponent->GetAvatarActor());
}

FGameplayTag UElectricCastleAbilitySystemLibrary::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTag(FElectricCastleGameplayTags::Get().Abilities))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UElectricCastleAbilitySystemLibrary::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FElectricCastleGameplayTags::Get().InputTag))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

TArray<FGameplayTag> UElectricCastleAbilitySystemLibrary::GetInputTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	TArray<FGameplayTag> InputTags;
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FElectricCastleGameplayTags::Get().InputTag))
			{
				InputTags.Add(Tag);
			}
		}
	}
	return InputTags;
}

int32 UElectricCastleAbilitySystemLibrary::GetXPReward(
	const UObject* WorldContextObject,
	const ECharacterClass& CharacterClass,
	const int32 Level
)
{
	if (const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(
		WorldContextObject
	))
	{
		return GameDataSubsystem->GetCharacterClassInfo()->GetXPReward(CharacterClass, Level);
	}
	UE_LOG(LogElectricCastle, Error, TEXT("Game mode is not set to ElectricCastleGameMode!"))
	return 0;
}

void UElectricCastleAbilitySystemLibrary::TraceForActorsInRadius(
	const UObject* WorldContextObject,
	const TArray<AActor*>& ActorsToIgnore,
	const FVector& SphereOrigin,
	const float Radius,
	TArray<AActor*>& OutOverlappingActors,
	const bool bDebug
)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	if (const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContextObject,
		EGetWorldErrorMode::LogAndReturnNull
	))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(
			Overlaps,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects),
			FCollisionShape::MakeSphere(Radius),
			SphereParams
		);
		if (bDebug)
		{
			DrawDebugSphere(World, SphereOrigin, Radius, 10, FColor::Red, false, 1.f, 0, 1.f);
		}
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (IsValid(Overlap.GetActor()))
			{
				OutOverlappingActors.Add(Overlap.GetActor());
			}
		}
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] No world found for context object %s!"),
			*FString("ElectricCastleAbilitySystemLibrary::GetLivePlayersWithinRadius"),
			*WorldContextObject->GetName()
		)
	}
}

void UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinRadius(
	const UObject* WorldContextObject,
	const TArray<AActor*>& ActorsToIgnore,
	const TArray<FName>& TagsToIgnore,
	const FVector& SphereOrigin,
	const float Radius,
	TArray<AActor*>& OutOverlappingActors,
	const bool bDebug
)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	if (const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContextObject,
		EGetWorldErrorMode::LogAndReturnNull
	))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(
			Overlaps,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius),
			SphereParams
		);
		if (bDebug)
		{
			DrawDebugSphere(World, SphereOrigin, Radius, 10, FColor::Red, false, 1.f, 0, 1.f);
		}
		FilterHitOverlaps(TagsToIgnore, OutOverlappingActors, Overlaps);
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] No world found for context object %s!"),
			*FString("ElectricCastleAbilitySystemLibrary::GetLivePlayersWithinRadius"),
			*WorldContextObject->GetName()
		)
	}
}

void UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinSweepRadius(
	const UObject* WorldContextObject,
	const TArray<AActor*>& ActorsToIgnore,
	const TArray<FName>& TagsToIgnore,
	const FVector& SphereStart,
	const FVector& SphereEnd,
	const float Radius,
	TArray<AActor*>& OutOverlappingActors,
	const bool bDebug
)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	if (const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContextObject,
		EGetWorldErrorMode::LogAndReturnNull
	))
	{
		TArray<FHitResult> Overlaps;
		World->SweepMultiByChannel(
			Overlaps,
			SphereStart,
			SphereEnd,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(Radius),
			SphereParams
		);
		if (bDebug)
		{
			UE_LOG(
				LogElectricCastle,
				Warning,
				TEXT(
					"[ElectricCastleAbilitySystemLibrary::GetLiveActorsWithinSweepRadius]: Found actors: [%d] between %s - %s"
				),
				Overlaps.Num(),
				*SphereStart.ToString(),
				*SphereEnd.ToString()
			)
			DrawDebugSweptSphere(World, SphereStart, SphereEnd, Radius, FColor::Red, false, 1.f);
		}
		FilterHitOverlaps(TagsToIgnore, OutOverlappingActors, Overlaps);
	}
	else if (bDebug)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] No world found for context object %s!"),
			*FString("ElectricCastleAbilitySystemLibrary::GetLivePlayersWithinSweepRadius"),
			*WorldContextObject->GetName()
		)
	}
}

void UElectricCastleAbilitySystemLibrary::GetClosestActors(
	const int32 MaxCount,
	const FVector& Location,
	const TArray<AActor*>& Actors,
	TArray<AActor*>& OutActors
)
{
	if (Actors.Num() <= MaxCount)
	{
		OutActors = Actors;
		return;
	}
	TArray<AActor*> SortedActors(Actors);
	SortedActors.Sort(
		[Location](AActor& A1, AActor& A2)
		{
			return
				FVector::Distance(Location, A1.GetActorLocation()) <
				FVector::Distance(
					Location,
					A2.GetActorLocation()
				);
		}
	);
	for (AActor* Actor : SortedActors)
	{
		if (OutActors.Num() < MaxCount)
		{
			OutActors.Add(Actor);
		}
		else
		{
			break;
		}
	}
}

bool UElectricCastleAbilitySystemLibrary::CanEquipAbility(
	UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& AbilityTag
)
{
	const FGameplayTag StatusTag = GetStatusTagByAbilityTag(AbilitySystemComponent, AbilityTag);
	FGameplayTagContainer EquippableStatuses;
	EquippableStatuses.AddTag(FElectricCastleGameplayTags::Get().Abilities_Status_Unlocked);
	EquippableStatuses.AddTag(FElectricCastleGameplayTags::Get().Abilities_Status_Equipped);
	return StatusTag.MatchesAny(EquippableStatuses);
}

bool UElectricCastleAbilitySystemLibrary::AbilityHasSlotTag(
	const FGameplayAbilitySpec& AbilitySpec,
	const FGameplayTag& SlotTag
)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTagExact(SlotTag))
		{
			return true;
		}
	}
	return false;
}

bool UElectricCastleAbilitySystemLibrary::AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.GetDynamicSpecSourceTags().HasTag(FElectricCastleGameplayTags::Get().InputTag);
}

FActiveGameplayEffectHandle UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(
	const FDamageEffectParams& DamageEffectParams
)
{
	if (!DamageEffectParams.SourceAbilitySystemComponent)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[UElectricCastleAbilitySystemLibrary::ApplyDamageEffect] No source ability system set on damage effect params!"))
		return FActiveGameplayEffectHandle();
	}
	if (!DamageEffectParams.TargetAbilitySystemComponent)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[UElectricCastleAbilitySystemLibrary::ApplyDamageEffect] No target ability system set on damage effect params!"))
		return FActiveGameplayEffectHandle();
	}
	if (!DamageEffectParams.DamageGameplayEffectClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[UElectricCastleAbilitySystemLibrary::ApplyDamageEffect] No damage effect class system set on damage effect params!"))
		return FActiveGameplayEffectHandle();
	}
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent ? DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass,
		DamageEffectParams.AbilityLevel,
		EffectContextHandle
	);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackVector(EffectContextHandle, DamageEffectParams.KnockbackForce);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);
	if (DamageEffectParams.bIsRadialDamage)
	{
		SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
		SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
		SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	}
	SpecHandle.Data->AddDynamicAssetTag(DamageEffectParams.DamageType);
	SpecHandle.Data->AppendDynamicAssetTags(DamageEffectParams.AbilityAssetTags);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Effect_Magnitude,
		DamageEffectParams.DamageMagnitude
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Effect_Debuff_Stat_Chance,
		DamageEffectParams.DebuffChance
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Effect_Debuff_Stat_Damage,
		DamageEffectParams.DebuffDamage
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Effect_Debuff_Stat_Duration,
		DamageEffectParams.DebuffDuration
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Effect_Debuff_Stat_Frequency,
		DamageEffectParams.DebuffFrequency
	);
	return DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

TArray<FRotator> UElectricCastleAbilitySystemLibrary::EvenlySpacedRotators(
	const FVector& Forward,
	const FVector& Axis,
	const float Spread,
	const int32 Count
)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (Count > 1)
	{
		const float DeltaSpread = Spread / (Count - 1);
		for (int32 i = 0; i < Count; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UElectricCastleAbilitySystemLibrary::EvenlyRotatedVectors(
	const FVector& Forward,
	const FVector& Axis,
	const float Spread,
	const int32 Count
)
{
	TArray<FVector> Vectors;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (Count > 1)
	{
		const float DeltaSpread = Spread / (Count - 1);
		for (int32 i = 0; i < Count; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}

FPredictionKey UElectricCastleAbilitySystemLibrary::GetPredictionKeyFromAbilitySpec(
	const FGameplayAbilitySpec& AbilitySpec
)
{
	if (TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances(); Instances.Num() > 0)
	{
		const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();
		return ActivationInfo.GetActivationPredictionKey();
	}
	return FPredictionKey();
}

FGameplayTag UElectricCastleAbilitySystemLibrary::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FElectricCastleGameplayTags::Get().Abilities_Status))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UElectricCastleAbilitySystemLibrary::GetStatusTagByAbilityTag(
	UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& AbilityTag
)
{
	if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusTagFromSpec(*Spec);
	}
	return FElectricCastleGameplayTags::Get().Abilities_Status_Locked;
}

FGameplayTag UElectricCastleAbilitySystemLibrary::GetInputTagByAbilityTag(
	UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& AbilityTag
)
{
	if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UElectricCastleAbilitySystemLibrary::IsAbilityEquipped(
	UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& AbilityTag
)
{
	return GetStatusTagByAbilityTag(AbilitySystemComponent, AbilityTag).MatchesTagExact(
		FElectricCastleGameplayTags::Get().Abilities_Status_Equipped
	);
}

float UElectricCastleAbilitySystemLibrary::GetAbilityPreferredDistanceByAbilityTag(AActor* Actor, const FGameplayTag& AbilityTag)
{
	return URandRangeBlueprintLibrary::GetRandomFloatInRange(GetAbilityPreferredDistanceRangeByAbilityTag(Actor, AbilityTag));
}

FFloatRange UElectricCastleAbilitySystemLibrary::GetAbilityPreferredDistanceRangeByAbilityTag(AActor* Actor, const FGameplayTag& AbilityTag)
{
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent(Actor))
	{
		const FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag);
		if (const UElectricCastleGameplayAbility* Ability = Spec ? Cast<UElectricCastleGameplayAbility>(Spec->Ability) : nullptr)
		{
			return Ability->GetPreferredDistanceRange();
		}
	}
	return FFloatRange();
}

bool UElectricCastleAbilitySystemLibrary::IsAbilitySupportedInEngagementRange(const FGameplayAbilitySpec& AbilitySpec, const EEngagementRange Range)
{
	if (const UElectricCastleGameplayAbility* Ability = Cast<UElectricCastleGameplayAbility>(AbilitySpec.Ability))
	{
		return Ability->IsSupportedEngagementRange(Range);
	}
	return true;
}

bool UElectricCastleAbilitySystemLibrary::IsAbilitySupportedInEngagementModes(const FGameplayAbilitySpec& AbilitySpec, const TArray<EEngagementAbilityMode> AbilityModes)
{
	if (const UElectricCastleGameplayAbility* Ability = Cast<UElectricCastleGameplayAbility>(AbilitySpec.Ability))
	{
		return Ability->IsSupportedEngagementMode(AbilityModes);
	}
	return true;
}

bool UElectricCastleAbilitySystemLibrary::DoesActorHaveGameplayTag(AActor* Actor, const FGameplayTag& Tag)
{
	if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		return AbilitySystemComponent->HasMatchingGameplayTag(Tag);
	}
	return false;
}

FGameplayAbilitySpec UElectricCastleAbilitySystemLibrary::MakeAbilitySpecFromAbilityInfo(const FElectricCastleAbilityInfo& AbilityInfo, const int32 Level)
{
	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityInfo.Ability, Level);
	const TArray TagsToAdd = {AbilityInfo.AbilityTag, AbilityInfo.AbilityType, AbilityInfo.InputTag, AbilityInfo.StatusTag, AbilityInfo.CooldownTag};
	for (const FGameplayTag& Tag : TagsToAdd)
	{
		if (Tag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Tag);
		}
	}
	return Spec;
}

bool UElectricCastleAbilitySystemLibrary::DoesGameplayAbilitySpecHaveTag(const FGameplayAbilitySpec& Spec, const FGameplayTag& Tag)
{
	return Spec.GetDynamicSpecSourceTags().HasTagExact(Tag);
}

bool UElectricCastleAbilitySystemLibrary::CanActorBlockByAngle(
	const FVector& AttackOrigin,
	const FVector& TargetLocation,
	const FVector& TargetForwardVector,
	const float BlockAllowedAngle,
	const bool bDebug
)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();

	const FVector ToAttack = (AttackOrigin - TargetLocation).GetSafeNormal();
	const float Dot = FVector::DotProduct(TargetForwardVector, ToAttack);

	// Convert allowed angle to a dot threshold
	const float HalfAngleRadians = FMath::DegreesToRadians(BlockAllowedAngle * 0.5f);
	const float DotThreshold = FMath::Cos(HalfAngleRadians);

	// --- Debug Visualization ---
	if (bDebug)
	{
		// Forward vector
		DrawDebugLine(
			World,
			TargetLocation,
			TargetLocation + TargetForwardVector * 150.f,
			FColor::Green,
			false,
			1.f,
			0,
			2.f
		);

		// ToAttack vector
		DrawDebugLine(
			World,
			TargetLocation,
			TargetLocation + ToAttack * 150.f,
			FColor::Red,
			false,
			1.f,
			0,
			2.f
		);
		// Cone boundaries
		const FVector LeftBoundary =
			TargetForwardVector.RotateAngleAxis(BlockAllowedAngle * 0.5f, FVector::UpVector);

		const FVector RightBoundary =
			TargetForwardVector.RotateAngleAxis(-BlockAllowedAngle * 0.5f, FVector::UpVector);

		DrawDebugLine(
			World,
			TargetLocation,
			TargetLocation + LeftBoundary * 150.f,
			FColor::Blue,
			false,
			1.f,
			0,
			1.f
		);

		DrawDebugLine(
			World,
			TargetLocation,
			TargetLocation + RightBoundary * 150.f,
			FColor::Blue,
			false,
			1.f,
			0,
			1.f
		);

		// Dot + threshold
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.f,
			FColor::Yellow,
			FString::Printf(TEXT("Dot: %.3f | Threshold: %.3f"), Dot, DotThreshold)
		);
	}

	return Dot >= DotThreshold;
}

AElectricCastleHUD* UElectricCastleAbilitySystemLibrary::GetElectricCastleHUD(const UObject* WorldContextObject)
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return Cast<AElectricCastleHUD>(PlayerController->GetHUD());
	}
	return nullptr;
}

void UElectricCastleAbilitySystemLibrary::ApplyGameplayEffectSpec(
	const UAbilitySystemComponent* SourceAbilitySystemComponent,
	UAbilitySystemComponent* TargetAbilitySystemComponent,
	const TSubclassOf<UGameplayEffect>& GameplayEffectClass,
	const float Level,
	const FMakeEffectSpecSignature* SetPropsOnSpecCallback
)
{
	FGameplayEffectContextHandle ContextHandle = SourceAbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceAbilitySystemComponent->GetAvatarActor());
	FGameplayEffectSpecHandle EffectSpec = SourceAbilitySystemComponent->MakeOutgoingSpec(
		GameplayEffectClass,
		Level,
		ContextHandle
	);
	if (SetPropsOnSpecCallback)
	{
		SetPropsOnSpecCallback->ExecuteIfBound(EffectSpec);
	}
	TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
}

bool UElectricCastleAbilitySystemLibrary::IsPassiveAbility(
	const UObject* WorldContextObject,
	const FGameplayAbilitySpec& AbilitySpec
)
{
	UAbilityInfo* AbilityInfo = GetAbilityInfo(WorldContextObject);
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(AbilitySpec);
	const FElectricCastleAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	return Info.AbilityType == FElectricCastleGameplayTags::Get().Abilities_Type_Passive;
}

void UElectricCastleAbilitySystemLibrary::SetKnockbackDirection(
	FDamageEffectParams& DamageEffectParams,
	FVector KnockbackDirection,
	const bool bOverrideMagnitude,
	const float Magnitude
)
{
	const float UseMagnitude = bOverrideMagnitude
		                           ? DamageEffectParams.KnockbackForceMagnitude
		                           : Magnitude;
	KnockbackDirection.Normalize();
	DamageEffectParams.KnockbackForce = KnockbackDirection * UseMagnitude;
}

void UElectricCastleAbilitySystemLibrary::SetDeathImpulseDirection(
	FDamageEffectParams& DamageEffectParams,
	FVector DeathImpulseDirection,
	const bool bOverrideMagnitude,
	const float Magnitude
)
{
	const float UseMagnitude = bOverrideMagnitude
		                           ? DamageEffectParams.DeathImpulseMagnitude
		                           : Magnitude;
	DeathImpulseDirection.Normalize();
	DamageEffectParams.DeathImpulse = DeathImpulseDirection * UseMagnitude;
}

FDamageEffectParams UElectricCastleAbilitySystemLibrary::MakeCustomDamageEffectParams(
	AActor* SourceActor,
	AActor* TargetActor,
	TSubclassOf<UGameplayEffect> DamageEffectClass,
	const FElectricCastleDamageConfig& InDamageConfig,
	int32 AbilityLevel,
	const FGameplayTagContainer& InAbilityAssetTags,
	FVector RadialDamageOrigin,
	bool bOverrideKnockbackDirection,
	FVector InKnockbackDirectionOverride,
	bool bOverrideDeathImpulse,
	FVector InDeathImpulseDirectionOverride,
	bool bOverridePitch,
	float PitchOverride
)
{
	FDamageEffectParams DamageEffectParams;
	DamageEffectParams.RadialDamageOrigin = RadialDamageOrigin;
	DamageEffectParams.FillFromDamageConfig(InDamageConfig);
	DamageEffectParams.WorldContextObject = SourceActor;
	DamageEffectParams.DamageGameplayEffectClass = DamageEffectClass;
	DamageEffectParams.SourceAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		SourceActor
	);
	DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	);
	DamageEffectParams.AbilityLevel = AbilityLevel;
	DamageEffectParams.DamageMagnitude = InDamageConfig.GetDamageMagnitudeAtLevel(AbilityLevel);
	DamageEffectParams.AbilityAssetTags = InAbilityAssetTags;
	DamageEffectParams.IgnoreTags = ICombatInterface::GetTargetTagsToIgnore(SourceActor);
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - SourceActor->GetActorLocation())
			.Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		else if (IsValid(TargetActor))
		{
			Rotation.Pitch = 25.f;
		}
		if (!bOverrideKnockbackDirection)
		{
			if (DamageEffectParams.RollForKnockbackChance())
			{
				DamageEffectParams.KnockbackForce = Rotation.Vector() * DamageEffectParams.KnockbackForceMagnitude;
			}
		}
		if (!bOverrideDeathImpulse)
		{
			DamageEffectParams.DeathImpulse = Rotation.Vector() * DamageEffectParams.DeathImpulseMagnitude;
		}
	}
	if (bOverrideKnockbackDirection)
	{
		InKnockbackDirectionOverride.Normalize();
		DamageEffectParams.KnockbackForce = InKnockbackDirectionOverride * InDamageConfig.KnockbackForceMagnitude;
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = InKnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;
			DamageEffectParams.KnockbackForce = KnockbackRotation.Vector() * InDamageConfig.KnockbackForceMagnitude;
		}
	}
	if (bOverrideDeathImpulse)
	{
		InDeathImpulseDirectionOverride.Normalize();
		DamageEffectParams.DeathImpulse = InDeathImpulseDirectionOverride * InDamageConfig.DeathImpulseMagnitude;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = InDeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;
			DamageEffectParams.DeathImpulse = DeathImpulseRotation.Vector() * InDamageConfig.DeathImpulseMagnitude;
		}
	}
	return DamageEffectParams;
}

FActiveGameplayEffectHandle UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(
	AActor* TargetActor,
	TSubclassOf<UGameplayEffect> GameplayEffect,
	const int32 Level
)
{
	if (!IsValid(TargetActor) || !IsValid(GameplayEffect))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("Requested to apply effect to either invalid actor or effect!"));
		return FActiveGameplayEffectHandle();
	}
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	))
	{
		FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystem->MakeEffectContext();
		EffectContextHandle.AddSourceObject(TargetActor);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(
			GameplayEffect,
			Level,
			EffectContextHandle
		);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(
			*EffectSpecHandle.Data.Get()
		);
		return ActiveEffectHandle;
	}
	UE_LOG(
		LogElectricCastle,
		Error,
		TEXT("[%s] Cannot apply basic gameplay effect - no AbilitySystem available on actor! [%s]"),
		*FString("UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect"),
		*TargetActor->GetName()
	)
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(
	AActor* TargetActor,
	const TSubclassOf<UGameplayEffect> GameplayEffect,
	const int32 Level,
	const FGameplayTag& MagnitudeTag,
	const float Magnitude
)
{
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystem->MakeEffectContext();
		EffectContextHandle.AddSourceObject(TargetActor);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(
			GameplayEffect,
			Level,
			EffectContextHandle
		);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(MagnitudeTag, Magnitude);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(
			*EffectSpecHandle.Data.Get()
		);
		return ActiveEffectHandle;
	}
	return FActiveGameplayEffectHandle();
}

void UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(
	AActor* TargetActor,
	FActiveGameplayEffectHandle& GameplayEffectHandle,
	const bool bRemoveAll,
	const bool bInvalidate
)
{
	if (!GameplayEffectHandle.IsValid())
	{
		return;
	}
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(
			GameplayEffectHandle,
			bRemoveAll
				? -1
				: 1
		);
		if (bInvalidate)
		{
			GameplayEffectHandle.Invalidate();
		}
	}
}

int32 UElectricCastleAbilitySystemLibrary::GetAbilityLevelByAbilityTag(
	AActor* Actor,
	const FGameplayTag& AbilityTag
)
{
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor)
	))
	{
		if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag))
		{
			return AbilitySpec->Level;
		}
	}
	return 0;
}

AActor* UElectricCastleAbilitySystemLibrary::FindHitBySphereTrace(
	const AActor* Player,
	const FSphereTraceParams& TraceParams,
	FHitResult& OutHitResult
)
{
	if (!IsValid(Player))
	{
		return nullptr;
	}
	const UWorld* World = Player->GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}
	const FVector Start = Player->GetActorLocation();
	const FVector End = Start + Player->GetActorForwardVector() * TraceParams.TraceDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player); // Don’t hit yourself
	Params.AddIgnoredActors(TraceParams.ActorsToIgnore);
	if (TraceParams.bDebug)
	{
		DrawDebugSweptSphere(World, Start, End, TraceParams.TraceRadius, FColor::Yellow, false, 5);
	}
	if (World->SweepSingleByChannel(
		OutHitResult,
		Start,
		End,
		FQuat::Identity,
		TraceParams.TraceChannel,
		FCollisionShape::MakeSphere(TraceParams.TraceRadius),
		Params
	))
	{
		return OutHitResult.GetActor(); // The first actor hit in front
	}

	return nullptr;
}

AActor* UElectricCastleAbilitySystemLibrary::FindHitByLineTrace(
	const AActor* Player,
	const FLineTraceParams& TraceParams
)
{
	if (!IsValid(Player))
	{
		return nullptr;
	}
	UWorld* World = Player->GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}
	const FVector Start = Player->GetActorLocation();
	const FVector End = Start + Player->GetActorForwardVector() * TraceParams.TraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bDebugQuery = TraceParams.bDebug;
	Params.AddIgnoredActor(Player); // Don’t hit yourself

	if (World->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		TraceParams.TraceChannel,
		// Trace against pawns (enemies usually derive from APawn)
		Params
	))
	{
		return HitResult.GetActor(); // The first actor hit in front
	}

	return nullptr;
}

bool UElectricCastleAbilitySystemLibrary::CalculatePitchToHitTarget(
	const FVector& Start,
	const FVector& Target,
	const float ProjectileSpeed,
	float& OutPitchDegrees
)
{
	const FVector Delta = Target - Start;

	constexpr float g = 980.0f; // cm/s^2
	const float x = FVector(Delta.X, Delta.Y, 0).Size(); // horizontal distance
	const float y = Delta.Z; // vertical difference
	const float v = ProjectileSpeed;

	const float v2 = v * v;
	const float v4 = v2 * v2;

	const float Discriminant = v4 - g * (g * x * x + 2 * y * v2);

	if (Discriminant < 0)
	{
		return false; // No valid solution (target out of range)
	}

	const float SqrtDisc = FMath::Sqrt(Discriminant);

	// Two possible angles: high arc and low arc
	const float TanTheta = (v2 - SqrtDisc) / (g * x);

	const float Theta = FMath::Atan(TanTheta); // radians
	OutPitchDegrees = FMath::RadiansToDegrees(Theta);

	return true;
}

bool UElectricCastleAbilitySystemLibrary::GetActorGroundLocation(AActor* Actor, FVector& OutGroundLocation)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[UElectricCastleAbilitySystemLibrary::GetActorGroundLocation] Provided invalid actor"))
		return false;
	}
	// Start and End points of the line trace
	FVector Start = Actor->GetActorLocation();
	FVector End = Start + FVector::DownVector * 10000;

	// Hit result to store information about what the line trace hits
	FHitResult HitResult;

	// Collision query parameters
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Actor); // Ignore self

	// Perform the line trace
	bool bHit = Actor->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	// Debug line to visualize the trace
	DrawDebugLine(Actor->GetWorld(), Start, End, FColor::Green, false, 2.0f);

	// Check if something was hit
	OutGroundLocation = HitResult.ImpactPoint;
	return bHit;
}

bool UElectricCastleAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UElectricCastleAbilitySystemLibrary::IsEvadedAttack(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsEvadedAttack();
	}
	return false;
}

bool UElectricCastleAbilitySystemLibrary::IsParriedAttack(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* LocalEffectContextHandle = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return LocalEffectContextHandle->IsParriedAttack();
	}
	return false;
}

bool UElectricCastleAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

bool UElectricCastleAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsRadialDamage();
	}
	return false;
}

float UElectricCastleAbilitySystemLibrary::GetRadialDamageInnerRadius(
	const FGameplayEffectContextHandle& EffectContextHandle
)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UElectricCastleAbilitySystemLibrary::GetRadialDamageOuterRadius(
	const FGameplayEffectContextHandle& EffectContextHandle
)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UElectricCastleAbilitySystemLibrary::GetRadialDamageOrigin(
	const FGameplayEffectContextHandle& EffectContextHandle
)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UElectricCastleAbilitySystemLibrary::SetIsRadialDamage(
	FGameplayEffectContextHandle& EffectContextHandle,
	const bool InIsRadialDamage
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->SetIsRadialDamage(InIsRadialDamage);
	}
}

void UElectricCastleAbilitySystemLibrary::SetRadialDamageInnerRadius(
	FGameplayEffectContextHandle& EffectContextHandle,
	const float InRadialDamageInnerRadius
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
	}
}

void UElectricCastleAbilitySystemLibrary::SetRadialDamageOuterRadius(
	FGameplayEffectContextHandle& EffectContextHandle,
	const float InRadialDamageOuterRadius
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

void UElectricCastleAbilitySystemLibrary::SetRadialDamageOrigin(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InRadialDamageOrigin
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->SetRadialDamageOrigin(InRadialDamageOrigin);
	}
}

bool UElectricCastleAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UElectricCastleAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

int32 UElectricCastleAbilitySystemLibrary::GetDebuffLevel(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffLevel();
	}
	return 0;
}

float UElectricCastleAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UElectricCastleAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UElectricCastleAbilitySystemLibrary::GetDebuffTypeTag(
	const FGameplayEffectContextHandle& EffectContextHandle
)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return *AuraEffectContext->GetDebuffTypeTag();
	}
	return FGameplayTag();
}

FGameplayTag UElectricCastleAbilitySystemLibrary::GetDamageTypeTag(
	const FGameplayEffectContextHandle& EffectContextHandle
)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return *AuraEffectContext->GetDamageTypeTag();
	}
	return FGameplayTag();
}

FVector UElectricCastleAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UElectricCastleAbilitySystemLibrary::GetKnockbackVector(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<const
		FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetKnockbackVector();
	}
	return FVector::ZeroVector;
}

void UElectricCastleAbilitySystemLibrary::SetIsBlockedHit(
	FGameplayEffectContextHandle& EffectContextHandle,
	const bool bInIsBlocked
)
{
	if (FElectricCastleGameplayEffectContext* LocalEffectContextHandle = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		LocalEffectContextHandle->SetIsBlockedHit(bInIsBlocked);
	}
}

void UElectricCastleAbilitySystemLibrary::SetIsEvadedAttack(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsEvaded)
{
	if (FElectricCastleGameplayEffectContext* LocalEffectContextHandle = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		LocalEffectContextHandle->SetIsEvadedAttack(bInIsEvaded);
	}
}

void UElectricCastleAbilitySystemLibrary::SetIsParriedAttack(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsParried)
{
	if (FElectricCastleGameplayEffectContext* LocalEffectContextHandle = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		LocalEffectContextHandle->SetIsParriedAttack(bInIsParried);
	}
}

void UElectricCastleAbilitySystemLibrary::SetIsCriticalHit(
	FGameplayEffectContextHandle& EffectContextHandle,
	const bool InIsCriticalHit
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(InIsCriticalHit);
	}
}

void UElectricCastleAbilitySystemLibrary::SetDebuff(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& DebuffTypeTag,
	const int32 Level,
	const float DebuffDamage,
	const float DebuffDuration,
	const float DebuffFrequency
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(true);
		AuraEffectContext->SetDebuffLevel(Level);
		AuraEffectContext->SetDebuffDamage(DebuffDamage);
		AuraEffectContext->SetDebuffDuration(DebuffDuration);
		AuraEffectContext->SetDebuffFrequency(DebuffFrequency);
		AuraEffectContext->SetDebuffTypeTag(MakeShared<FGameplayTag>(DebuffTypeTag));
	}
}

void UElectricCastleAbilitySystemLibrary::SetIsSuccessfulDebuff(
	FGameplayEffectContextHandle& EffectContextHandle,
	bool InIsSuccessfulDebuff
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(InIsSuccessfulDebuff);
	}
}

void UElectricCastleAbilitySystemLibrary::SetDebuffDamage(
	FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffDamage
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UElectricCastleAbilitySystemLibrary::SetDebuffDuration(
	FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffDuration
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UElectricCastleAbilitySystemLibrary::SetDebuffFrequency(
	FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffFrequency
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UElectricCastleAbilitySystemLibrary::SetDebuffTypeTag(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDebuffTypeTag
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffTypeTag(MakeShared<FGameplayTag>(InDebuffTypeTag));
	}
}

void UElectricCastleAbilitySystemLibrary::SetDamageTypeTag(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageTypeTag
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDamageTypeTag(MakeShared<FGameplayTag>(InDamageTypeTag));
	}
}

void UElectricCastleAbilitySystemLibrary::SetDeathImpulse(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InDeathImpulse
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UElectricCastleAbilitySystemLibrary::SetKnockbackVector(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InKnockbackVector
)
{
	if (FElectricCastleGameplayEffectContext* AuraEffectContext = static_cast<FElectricCastleGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockbackVector(InKnockbackVector);
	}
}

FActiveGameplayEffectHandle UElectricCastleAbilitySystemLibrary::ApplyDurationEffectByTag(
	AActor* Actor,
	const FGameplayTag DurationEffectTag,
	const float Duration,
	const int32 Level
)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		// Build a GameplayEffectSpec on the fly
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			UDurationGameplayEffect::StaticClass(),
			Level,
			AbilitySystemComponent->MakeEffectContext()
		);

		if (SpecHandle.IsValid())
		{
			if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
			{
				// Set duration
				Spec->SetDuration(Duration, true);
				// 1. Add as an asset tag (tag belongs to the GE itself)
				Spec->AddDynamicAssetTag(DurationEffectTag);
				// 2. Grant to the target actor (tag is applied to ASC while GE is active)
				Spec->DynamicGrantedTags.AddTag(DurationEffectTag);
				// Apply to self
				return AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
			}
		}
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(AActor* Actor, const FGameplayTag DurationEffectTag, const int32 Level)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		// Build a GameplayEffectSpec on the fly
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			UInfiniteGameplayEffect::StaticClass(),
			Level,
			AbilitySystemComponent->MakeEffectContext()
		);

		if (SpecHandle.IsValid())
		{
			if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
			{
				// 1. Add as an asset tag (tag belongs to the GE itself)
				Spec->AddDynamicAssetTag(DurationEffectTag);
				// 2. Grant to the target actor (tag is applied to ASC while GE is active)
				Spec->DynamicGrantedTags.AddTag(DurationEffectTag);
				// Apply to self
				return AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
			}
		}
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTags(AActor* Actor, const FGameplayTagContainer TagContainer, const int32 Level)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		// Build a GameplayEffectSpec on the fly
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			UInfiniteGameplayEffect::StaticClass(),
			Level,
			AbilitySystemComponent->MakeEffectContext()
		);

		if (SpecHandle.IsValid())
		{
			if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
			{
				for (const FGameplayTag& Tag : TagContainer)
				{
					// 1. Add as an asset tag (tag belongs to the GE itself)
					Spec->AddDynamicAssetTag(Tag);
					// 2. Grant to the target actor (tag is applied to ASC while GE is active)
					Spec->DynamicGrantedTags.AddTag(Tag);
				}
				// Apply to self
				return AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
			}
		}
	}
	return FActiveGameplayEffectHandle();
}
