// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StatusEffectPlugin.generated.h"

struct FStatusEffectDuration;
struct FGameplayTag;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API UStatusEffectPlugin : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyEffect(const FGameplayTag& StatusEffectTag, AActor* InActor, const FStatusEffectDuration& InDuration) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveEffect(const FGameplayTag& StatusEffectTag, AActor* InActor) const;
};
