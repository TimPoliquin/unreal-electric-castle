// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "EquipmentActor.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UEquipmentActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IEquipmentActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsEquipped() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FGameplayTag GetItemType() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Equip(AActor* InOwner);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unequip(AActor* InOwner);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Attach(AActor* InOwner);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Detach();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddImpulse(const FVector Impulse);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hide();

	static FGameplayTag GetItemType(const UObject* Object);
};
