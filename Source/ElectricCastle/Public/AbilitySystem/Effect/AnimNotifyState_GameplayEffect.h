// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "AnimNotifyState_GameplayEffect.generated.h"

struct FActiveGameplayEffectHandle;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAnimNotifyState_GameplayEffectConfigRow
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Level = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseMagnitude = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition=bUseMagnitude, EditConditionHides=true))
	FGameplayTag MagnitudeTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition=bUseMagnitude, EditConditionHides=true))
	float Magnitude = 1.f;

	FAnimNotifyState_GameplayEffectConfigRow()
	{
		MagnitudeTag = FElectricCastleGameplayTags::Get().Effect_Magnitude;
	}
};

/**
 * 
 */
UCLASS(DisplayName="Gameplay Effect State")
class ELECTRICCASTLE_API UAnimNotifyState_GameplayEffect : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Properties")
	TArray<FAnimNotifyState_GameplayEffectConfigRow> GameplayEffects;

private:
	TArray<FActiveGameplayEffectHandle> Handles;
};
