// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SignificanceSensitiveActor.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USignificanceSensitiveActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISignificanceSensitiveActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void EnterSignificance_FullySignificant();
	UFUNCTION(BlueprintNativeEvent)
	void EnterSignificance_PartiallySignificant();
	UFUNCTION(BlueprintNativeEvent)
	void EnterSignificance_Insignificant();

	static bool IsSignificanceSensitiveActor(const UObject* Object);
	static void EnterSignificance_FullySignificant(UObject* Object);
	static void EnterSignificance_PartiallySignificant(UObject* Object);
	static void EnterSignificance_Insignificant(UObject* Object);
};
