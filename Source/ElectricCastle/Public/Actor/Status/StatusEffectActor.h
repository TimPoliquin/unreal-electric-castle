// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatusEffectActor.generated.h"

class UStatusEffectManagerComponent;
// This class does not need to be modified.
UINTERFACE()
class UStatusEffectActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IStatusEffectActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UStatusEffectManagerComponent* GetStatusEffectManagerComponent() const;

	static UStatusEffectManagerComponent* GetStatusEffectManagerComponent(const UObject* Actor);
};
