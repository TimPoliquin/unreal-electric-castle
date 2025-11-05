// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageDealingActor.generated.h"

struct FDamageEffectParams;
// This class does not need to be modified.
UINTERFACE()
class UDamageDealingActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IDamageDealingActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyDamageEffectParams(const FDamageEffectParams& DamageEffectParams);

	static void ApplyDamageEffectParams(UObject* DamageDealingActor, const FDamageEffectParams& InDamageEffectParams);
};
