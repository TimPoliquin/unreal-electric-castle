// Copyright Alien Shores


#include "UI/Widget/Form/FormWheelWidgetManager.h"

#include "Components/PanelWidget.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "UI/Widget/Form/FormWheelWidget.h"


void UFormWheelWidgetManager::BindPlayerFormsViewModel(UMVVM_PlayerForms* ViewModel)
{
	if (!ViewModel)
	{
		return;
	}
	ViewModel->OnVisibilityChange.AddUniqueDynamic(this, &UFormWheelWidgetManager::HandlePlayerFormVisibilityChange);
}

void UFormWheelWidgetManager::HandlePlayerFormVisibilityChange(const FPlayerFormsVisibilityChangePayload& Payload)
{
	if (Payload.bIsVisible)
	{
		ShowFormWheelWidget(Payload.ViewModel);
	}
	else
	{
		HideFormWheelWidget(Payload.ViewModel);
	}
}

void UFormWheelWidgetManager::ShowFormWheelWidget(UMVVM_PlayerForms* ViewModel)
{
	// create if it doesn't exist
	if (!FormWheelWidgetMap.Contains(ViewModel->GetPlayerIndex()))
	{
		if (UFormWheelWidget* FormWheelWidget = CreateFormWheelWidget(ViewModel))
		{
			FormWheelWidgetMap.Add(ViewModel->GetPlayerIndex(), FormWheelWidget);
		}
	}
	// show if it exists now (may not exist due to configuration errors
	if (FormWheelWidgetMap.Contains(ViewModel->GetPlayerIndex()))
	{
		FormWheelWidgetMap[ViewModel->GetPlayerIndex()]->Show(true);
	}
}

void UFormWheelWidgetManager::HideFormWheelWidget(UMVVM_PlayerForms* ViewModel)
{
	if (!FormWheelWidgetMap.Contains(ViewModel->GetPlayerIndex()))
	{
		return;
	}
	FormWheelWidgetMap[ViewModel->GetPlayerIndex()]->Hide(true);
	FormWheelWidgetMap.Remove(ViewModel->GetPlayerIndex());
}

UFormWheelWidget* UFormWheelWidgetManager::CreateFormWheelWidget(UMVVM_PlayerForms* PlayerFormsViewModel)
{
	if (!FormWheelWidgetClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] FormWheelWidgetClass is null"), *GetName());
		return nullptr;
	}
	if (UPanelWidget* PanelWidget = GetFormWheelPanel())
	{
		if (UFormWheelWidget* FormWheelWidget = CreateWidget<UFormWheelWidget>(
			this,
			FormWheelWidgetClass,
			PlayerFormsViewModel->CreateWidgetName(FString("FormWheel"))
		))
		{
			FormWheelWidget->BindViewModel(PlayerFormsViewModel);
			FormWheelWidget->Hide(false);
			PanelWidget->AddChild(FormWheelWidget);
			return FormWheelWidget;
		}
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to create FormWheelWidget"), *GetName());
		return nullptr;
	}
	UE_LOG(LogElectricCastle, Error, TEXT("[%s] FormWheelParent is null"), *GetName());
	return nullptr;
}
