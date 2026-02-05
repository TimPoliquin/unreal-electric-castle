// Copyright Alien Shores


#include "UI/HUD/OverlayWidget.h"

#include <Components/PanelWidget.h>

#include "Components/NamedSlot.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "UI/HUD/ElectricCastleHUD.h"
#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "UI/Widget/FormWheelWidget.h"

class AElectricCastleHUD;

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

void UOverlayWidget::OnFormWheelVisibilityChange(const FOnPlayerFormWheelVisibilityChangePayload& Payload)
{
	if (Payload.IsVisible())
	{
		ShowFormWheel();
	}
	else
	{
		HideFormWheel();
	}
}

void UOverlayWidget::HideFormWheel_Implementation()
{
}

void UOverlayWidget::ShowFormWheel_Implementation()
{
}

UFormWheelWidget* UOverlayWidget::CreateFormWheelWidget_Implementation(
	UPanelWidget* ParentWidget,
	UMVVM_PlayerForms* PlayerFormsViewModel
)
{
	if (!FormWheelWidgetClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] FormWheelWidgetClass is null"), *GetName());
		return nullptr;
	}
	if (UFormWheelWidget* FormWheelWidget = CreateWidget<UFormWheelWidget>(
		this,
		FormWheelWidgetClass,
		PlayerFormsViewModel->CreateWidgetName(FString("FormWheel"))
	))
	{
		FormWheelWidget->BindViewModel(PlayerFormsViewModel);
		FormWheelWidget->Hide(false);
		FormWheelWidgets.Add(FormWheelWidget);
		ParentWidget->AddChild(FormWheelWidget);
		return FormWheelWidget;
	}
	UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to create FormWheelWidget"), *GetName());
	return nullptr;
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
	// nothing to do in native right now
}

UFormWheelWidget* UOverlayWidget::GetFormWheelWidgetByPlayerIndex(const int32 PlayerIndex) const
{
	if (FormWheelWidgets.IsValidIndex(PlayerIndex))
	{
		return *FormWheelWidgets.FindByPredicate(
			[PlayerIndex](const UFormWheelWidget* FormWheelWidget)
			{
				return FormWheelWidget->GetPlayerIndex() == PlayerIndex;
			}
		);
	}
	return nullptr;
}

void UOverlayWidget::Hide_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UOverlayWidget::Show_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}
