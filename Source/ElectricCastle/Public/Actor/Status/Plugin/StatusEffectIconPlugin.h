// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "StatusEffectPlugin.h"
#include "UObject/Object.h"
#include "StatusEffectIconPlugin.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UStatusEffectIconPlugin : public UStatusEffectPlugin
{
	GENERATED_BODY()

public:
	virtual void ApplyEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor, const FStatusEffectDuration& InDuration) const override;
	virtual void RemoveEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor EffectColor = FLinearColor::White;
};
