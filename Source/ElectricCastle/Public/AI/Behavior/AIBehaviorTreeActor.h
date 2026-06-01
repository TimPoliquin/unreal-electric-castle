// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIBehaviorTreeActor.generated.h"

class UBehaviorTree;
// This class does not need to be modified.
UINTERFACE()
class UAIBehaviorTreeActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IAIBehaviorTreeActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UBehaviorTree* GetBehaviorTree() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ShouldAutoRunBehaviorTree() const;

	static UBehaviorTree* GetBehaviorTree(const UObject* Object);
	static bool ShouldAutoRunBehaviorTree(const UObject* Object);
};
