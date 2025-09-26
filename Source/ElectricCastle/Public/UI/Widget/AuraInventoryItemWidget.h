// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraInventoryItemWidget.generated.h"

class UMVVM_InventoryItem;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAuraInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void BindViewModel(UMVVM_InventoryItem* InventoryItemViewModel);
};
