#pragma once
#include "CoreMinimal.h"
#include "ElectricCastleGameplayAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAbilityCooldownConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FScalableFloat Duration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Cooldown"))
	FGameplayTag CooldownTag = FGameplayTag::EmptyTag;

	bool IsValid() const
	{
		return CooldownTag.IsValid() && Duration.IsValid();
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FComboAbilityConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ComboSectionName = NAME_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Event.Montage"))
	FGameplayTag MontageEventTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsWeaponAbility = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Categories="Combat.Socket"))
	FGameplayTag SocketTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bOverrideAbilityImpactRadius = false;
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadWrite,
		meta=(EditCondition="bOverrideAbilityImpactRadius", EditConditionHides)
	)
	float ImpactRadius = 25.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CameraKick")
	bool bCameraKickOnHit = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CameraKick", meta=(EditCondition="bCameraKickOnHit", EditConditionHides))
	FVector KickDirection = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CameraKick", meta=(EditCondition="bCameraKickOnHit", EditConditionHides))
	FScalableFloat KickMagnitude;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CameraKick", meta=(EditCondition="bCameraKickOnHit", EditConditionHides))
	float KickDuration = .15f;
};
