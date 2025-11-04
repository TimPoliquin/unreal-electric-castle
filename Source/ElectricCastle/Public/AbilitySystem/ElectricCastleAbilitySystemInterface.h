// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ElectricCastleAbilitySystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UElectricCastleAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IElectricCastleAbilitySystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetCharacterLevel() const;
	static int32 GetCharacterLevel(const UObject* Character);
	static bool IsAbilitySystemReady(AActor* Actor);
};
