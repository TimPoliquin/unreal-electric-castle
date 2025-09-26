// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SwitchDelegates.h"
#include "UObject/Interface.h"
#include "SwitchInterface.generated.h"


// This class does not need to be modified.
UINTERFACE()
class USwitchInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISwitchInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FOnSwitchStatusChangedSignature& GetOnSwitchActivatedDelegate() = 0;
	virtual FOnSwitchStatusChangedSignature& GetOnSwitchDeactivatedDelegate() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsSwitchActive() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetSwitch();

	static bool IsSwitchActive(const AActor* Switch);
	static void ResetSwitch(AActor* Switch);
};
