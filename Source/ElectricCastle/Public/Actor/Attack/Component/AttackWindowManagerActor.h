// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackWindowManagerActor.generated.h"

class UAttackWindowManager;
// This class does not need to be modified.
UINTERFACE()
class UAttackWindowManagerActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IAttackWindowManagerActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAttackWindowManager* GetAttackWindowManager() const;

	static UAttackWindowManager* GetAttackWindowManager(const UObject* Actor);
};
