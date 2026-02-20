// Copyright Alien Shores


#include "UI/HUD/OverlayWidget.h"

#include <Components/PanelWidget.h>

#include "Components/NamedSlot.h"
#include "UI/HUD/ElectricCastleHUD.h"
#include "UI/Widget/Form/FormWheelWidgetManager.h"

TArray<UMVVM_PlayerState*> UOverlayWidget::GetPlayerStateViewModels() const
{
	if (const AElectricCastleHUD* HUD = Cast<AElectricCastleHUD>(GetOwningPlayer()->GetHUD()))
	{
		return HUD->GetPlayerStateViewModels();
	}
	return TArray<UMVVM_PlayerState*>();
}

void UOverlayWidget::ShowMajorNotification_Implementation(UUserWidget* NotificationWidget, UNamedSlot* NotificationSlot)
{
	NotificationSlot->AddChild(NotificationWidget);
}

void UOverlayWidget::BindPlayerStateViewModel_Implementation(UMVVM_PlayerState* ViewModel)
{
}

void UOverlayWidget::BindPlayerAbilityStatesViewModel_Implementation(UMVVM_PlayerAbilityStates* ViewModel)
{
	// nothing to do in native right now
}

void UOverlayWidget::BindPlayerFormsViewModel_Implementation(UMVVM_PlayerForms* ViewModel)
{
	if (UFormWheelWidgetManager* FormWheelWidgetManager = GetFormWheelWidgetManager())
	{
		FormWheelWidgetManager->BindPlayerFormsViewModel(ViewModel);
	}
}

void UOverlayWidget::Hide_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UOverlayWidget::Show_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}
