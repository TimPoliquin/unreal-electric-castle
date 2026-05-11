// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PatrollingActor.generated.h"

class UPatrolComponent;
// This class does not need to be modified.
UINTERFACE()
class UPatrollingActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IPatrollingActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UPatrolComponent* GetPatrolComponent() const;

	static UPatrolComponent* GetPatrolComponent(const UObject* Actor);
	static bool CanPatrol(const UObject* Actor);
};
