// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BeamTargetInterface.generated.h"

struct FGameplayTag;
// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UBeamTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IBeamTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleBeamApplied(const FGameplayTag& BeamType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleBeamRemoved(const FGameplayTag& BeamType);

	static bool IsBeamTarget(const UObject* Object);
};
