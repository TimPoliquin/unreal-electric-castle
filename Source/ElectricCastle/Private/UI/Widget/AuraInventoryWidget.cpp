// Copyright Alien Shores


#include "UI/Widget/AuraInventoryWidget.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "UI/HUD/ElectricCastleHUD.h"
#include "UI/ViewModel/MVVM_Inventory.h"
#include "UI/ViewModel/MVVM_InventoryItem.h"
#include "UI/Widget/AuraInventoryItemWidget.h"

TArray<UAuraInventoryItemWidget*> UAuraInventoryWidget::CreateInventoryItems(
	UUniformGridPanel* GridPanel,
	int32 Columns,
	int32 Rows
)
{
	GridPanel->ClearChildren();
	for (int32 Row = 0; Row < Rows; Row++)
	{
		for (int32 Column = 0; Column < Columns; Column++)
		{
			UAuraInventoryItemWidget* InventoryItem =
				CreateWidget<UAuraInventoryItemWidget>(
					this,
					InventoryItemWidgetClass,
					FName(*FString::Printf(TEXT("Inventory Item %d - %d"), Row, Column))
				);
			InventoryItemWidgets.Add(InventoryItem);
			if (UUniformGridSlot* InventoryItemSlot = GridPanel->AddChildToUniformGrid(InventoryItem))
			{
				InventoryItemSlot->SetRow(Row);
				InventoryItemSlot->SetColumn(Column);
				InventoryItemSlot->SetHorizontalAlignment(HAlign_Center);
				InventoryItemSlot->SetVerticalAlignment(VAlign_Center);
			}
		}
	}
	return InventoryItemWidgets;
}

UMVVM_Inventory* UAuraInventoryWidget::FindInventoryViewModel() const
{
	if (AElectricCastleHUD* HUD = Cast<AElectricCastleHUD>(GetOwningPlayer()->GetHUD()))
	{
		return HUD->GetInventoryViewModel();
	}
	return nullptr;
}

void UAuraInventoryWidget::BindViewModel_Implementation(UMVVM_Inventory* InventoryViewModel)
{
	if (!InventoryViewModel)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("InventoryViewModel is null"));
		return;
	}
	if (InventoryViewModel->GetInventoryItems().Num() != InventoryItemWidgets.Num())
	{
		UE_LOG(LogElectricCastle, Error, TEXT("Mismatch between InventoryItem view model and Widgets!"));
		return;
	}
	for (UMVVM_InventoryItem* InventoryItemModel : InventoryViewModel->GetInventoryItems())
	{
		InventoryItemWidgets[InventoryItemModel->GetItemIndex()]->BindViewModel(InventoryItemModel);
	}
}
