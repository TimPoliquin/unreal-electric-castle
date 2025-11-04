// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EffectAdd_Lightning();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EffectRemove_Lightning();

	static void EffectAdd_Lightning(UObject* Weapon);
	static void EffectRemove_Lightning(UObject* Weapon);
};
