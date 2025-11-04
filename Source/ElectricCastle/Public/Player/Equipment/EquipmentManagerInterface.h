// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipmentManagerInterface.generated.h"

class UPlayerEquipmentComponent;
// This class does not need to be modified.
UINTERFACE()
class UEquipmentManagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IEquipmentManagerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UPlayerEquipmentComponent* GetEquipmentComponent() const;

	static UPlayerEquipmentComponent* GetEquipmentComponent(const UObject* Object);
};
