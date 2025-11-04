// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraInventoryWidget.generated.h"

class UMVVM_Inventory;
class UAuraInventoryItemWidget;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAuraInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<UAuraInventoryItemWidget*> CreateInventoryItems(UUniformGridPanel* GridPanel, int32 Columns, int32 Rows);
	UFUNCTION(BlueprintCallable)
	UMVVM_Inventory* FindInventoryViewModel() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindViewModel(UMVVM_Inventory* InventoryViewModel);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> InventoryItemWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RowCount = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ColumnCount = 5;
	UPROPERTY(BlueprintReadOnly)
	TArray<UAuraInventoryItemWidget*> InventoryItemWidgets;
};
