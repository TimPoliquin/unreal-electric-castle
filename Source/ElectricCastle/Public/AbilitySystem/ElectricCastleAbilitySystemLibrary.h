// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ElectricCastleAbilitySystemTypes.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"
#include "Utils/TagUtils.h"
#include "Utils/TraceParams.h"
#include "ElectricCastleAbilitySystemLibrary.generated.h"

class ULootTiers;
DECLARE_DELEGATE_OneParam(FMakeEffectSpecSignature, FGameplayEffectSpecHandle&);

struct FDamageEffectParams;
class UElectricCastleAbilitySystemComponent;
class UAbilityInfo;
class USpellMenuWidgetController;
class UGameplayAbility;
struct FGameplayEffectSpecHandle;
struct FGameplayEffectContextHandle;
class UCharacterClassInfo;
class UGameplayEffect;
class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class AElectricCastleHUD;
struct FWidgetControllerParams;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintPure,
		Category= "ElectricCastleAbilitySystemLibrary|WidgetController",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	UFUNCTION(
		BlueprintPure,
		Category= "ElectricCastleAbilitySystemLibrary|WidgetController",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	UFUNCTION(
		BlueprintPure,
		Category= "ElectricCastleAbilitySystemLibrary|WidgetController",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	UFUNCTION(
		BlueprintCallable,
		BlueprintPure,
		Category= "ElectricCastleAbilitySystemLibrary|AbilitySystem",
		meta = (DefaultToSelf = "Actor")
	)
	static UElectricCastleAbilitySystemComponent* GetAbilitySystemComponent(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category= "ElectricCastleAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(
		const UObject* WorldContextObject,
		ECharacterClass CharacterClass,
		const float Level,
		UAbilitySystemComponent* AbilitySystemComponent
	);

	UFUNCTION(BlueprintCallable, Category = "ElectricCastleAbilitySystemLibrary|CharacterClassDefaults")
	static void GrantStartupAbilities(
		const UObject* WorldContextObject,
		UAbilitySystemComponent* AbilitySystemComponent,
		ECharacterClass CharacterClass,
		int Level
	);
	UFUNCTION(BlueprintCallable, Category = "ElectricCastleAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "ElectricCastleAbilitySystemLibrary|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ElectricCastleAbilitySystemLibrary|Attributes")
	static bool IsFullHealth(AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ElectricCastleAbilitySystemLibrary|Attributes")
	static bool IsFullMana(AActor* Actor);

	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static bool IsInfiniteEffect(const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static bool IsInstantEffect(const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool InIsRadialDamage);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageInnerRadius(
		FGameplayEffectContextHandle& EffectContextHandle,
		float InRadialDamageInnerRadius
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOuterRadius(
		FGameplayEffectContextHandle& EffectContextHandle,
		float InRadialDamageOuterRadius
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOrigin(
		FGameplayEffectContextHandle& EffectContextHandle,
		const FVector& InRadialDamageOrigin
	);

	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static int32 GetDebuffLevel(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDebuffTypeTag(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDamageTypeTag(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static FVector GetKnockbackVector(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetIsBlockedHit(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		bool InIsBlocked
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		bool InIsCriticalHit
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetDebuff(
		FGameplayEffectContextHandle& EffectContextHandle,
		const FGameplayTag& DebuffTypeTag,
		int32 Level,
		float DebuffDamage,
		float DebuffDuration,
		float DebuffFrequency
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetIsSuccessfulDebuff(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		bool InIsSuccessfulDebuff
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDamage(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		float InDebuffDamage
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDuration(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		float InDebuffDuration
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffFrequency(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		float InDebuffFrequency
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffTypeTag(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		const FGameplayTag& InDebuffTypeTag
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetDamageTypeTag(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		const FGameplayTag& InDamageTypeTag
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetDeathImpulse(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		const FVector& InDeathImpulse
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static void SetKnockbackVector(
		UPARAM(ref)
		FGameplayEffectContextHandle& EffectContextHandle,
		const FVector& InKnockbackVector
	);

	static int GetCharacterLevel(UAbilitySystemComponent* AbilitySystemComponent);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static TArray<FGameplayTag> GetInputTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagByAbilityTag(
		UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& AbilityTag
	);
	static FGameplayTag GetInputTagByAbilityTag(
		UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& AbilityTag
	);
	static bool IsAbilityEquipped(
		UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& AbilityTag
	);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static int32 GetXPReward(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, int32 Level);
	template <typename HitResult>
	static void FilterHitOverlaps(
		const TArray<FName>& TagsToIgnore,
		TArray<AActor*>& OutOverlappingActors,
		TArray<HitResult> Overlaps
	);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static void GetLiveActorsWithinRadius(
		const UObject* WorldContextObject,
		const TArray<AActor*>& ActorsToIgnore,
		const TArray<FName>& TagsToIgnore,
		const FVector& SphereOrigin,
		const float Radius,
		TArray<AActor*>& OutOverlappingActors,
		const bool bDebug = false
	);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static void GetLiveActorsWithinSweepRadius(
		const UObject* WorldContextObject,
		const TArray<AActor*>& ActorsToIgnore,
		const TArray<FName>& TagsToIgnore,
		const FVector& SphereStart,
		const FVector& SphereEnd,
		const float Radius,
		TArray<AActor*>& OutOverlappingActors,
		const bool bDebug = false
	);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestActors(
		int32 MaxCount,
		const FVector& Location,
		const TArray<AActor*>& Actors,
		TArray<AActor*>& OutActors
	);

	static bool CanEquipAbility(
		UElectricCastleAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& AbilityTag
	);
	static bool AbilityHasSlotTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& SlotTag);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(
		const FVector& Forward,
		const FVector& Axis,
		const float Spread,
		const int32 Count
	);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(
		const FVector& Forward,
		const FVector& Axis,
		const float Spread,
		const int32 Count
	);
	static FPredictionKey GetPredictionKeyFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);
	static void ApplyGameplayEffectSpec(
		const UAbilitySystemComponent* SourceAbilitySystemComponent,
		UAbilitySystemComponent* TargetAbilitySystemComponent,
		const TSubclassOf<UGameplayEffect>& GameplayEffectClass,
		float Level,
		const FMakeEffectSpecSignature* SetPropsOnSpecCallback = nullptr
	);

	static bool IsPassiveAbility(const UObject* WorldContextObject, const FGameplayAbilitySpec& AbilitySpec);

	/** Damage effect params **/
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static void SetKnockbackDirection(
		UPARAM(ref)
		FDamageEffectParams& DamageEffectParams,
		FVector KnockbackDirection,
		const bool bOverrideMagnitude = false,
		const float Magnitude = 0.f
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static void SetDeathImpulseDirection(
		UPARAM(ref)
		FDamageEffectParams& DamageEffectParams,
		FVector DeathImpulseDirection,
		const bool bOverrideMagnitude = false,
		const float Magnitude = 0.f
	);

	static FDamageEffectParams MakeCustomDamageEffectParams(
		AActor* SourceActor,
		AActor* TargetActor,
		TSubclassOf<UGameplayEffect> DamageEffectClass,
		const FElectricCastleDamageConfig& InDamageConfig,
		int32 AbilityLevel,
		const FGameplayTagContainer& AbilityAssetTags,
		FVector RadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector InKnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector InDeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f
	);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static FActiveGameplayEffectHandle ApplyBasicGameplayEffect(
		AActor* TargetActor,
		TSubclassOf<UGameplayEffect> GameplayEffect,
		int32 Level = 1
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static FActiveGameplayEffectHandle ApplyBasicGameplayEffectWithMagnitude(
		AActor* TargetActor,
		TSubclassOf<UGameplayEffect> GameplayEffect,
		int32 Level,
		const FGameplayTag& MagnitudeTag,
		const float Magnitude
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static void RemoveGameplayEffect(
		AActor* TargetActor,
		const FActiveGameplayEffectHandle& GameplayEffectHandle,
		bool bRemoveAll = true
	);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static int32 GetAbilityLevelByAbilityTag(AActor* Actor, const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static AActor* FindHitBySphereTrace(const AActor* Player, const FSphereTraceParams& TraceParams);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static AActor* FindHitByLineTrace(const AActor* Player, const FLineTraceParams& TraceParams);
	UFUNCTION(BlueprintCallable, Category="ElectricCastleAbilitySystemLibrary|GameplayMechanics")
	static bool CalculatePitchToHitTarget(
		const FVector& Start,
		const FVector& Target,
		const float ProjectileSpeed,
		float& OutPitchDegrees
	);

private:
	static bool GetWidgetControllerParams(
		const UObject* WorldContextObject,
		FWidgetControllerParams& FWidgetControllerParams
	);
	static AElectricCastleHUD* GetElectricCastleHUD(const UObject* WorldContextObject);

	static void GrantAbilities(
		UAbilitySystemComponent* AbilitySystemComponent,
		const TArray<TSubclassOf<UGameplayAbility>>& Abilities,
		int Level
	);
};

template <typename HitResult>
void UElectricCastleAbilitySystemLibrary::FilterHitOverlaps(
	const TArray<FName>& TagsToIgnore,
	TArray<AActor*>& OutOverlappingActors,
	TArray<HitResult> Overlaps
)
{
	for (const HitResult& Overlap : Overlaps)
	{
		AActor* OverlapActor = Overlap.GetActor();
		// skip actor if it has any of the tags in the ignore list
		if (TagUtils::HasAnyTag(OverlapActor, TagsToIgnore))
		{
			continue;
		}
		if (ICombatInterface::IsAlive(Overlap.GetActor()))
		{
			OutOverlappingActors.AddUnique(OverlapActor);
		}
	}
}
