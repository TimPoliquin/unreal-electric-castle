// Copyright Alien Shores


#include "UI/HUD/OverlayWidget.h"

#include <Components/PanelWidget.h>

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/ElectricCastlePlayerState.h"
#include "UI/HUD/ElectricCastleHUD.h"
#include "UI/Library/WidgetFunctionLibrary.h"
#include "UI/ViewModel/Form/MVVM_PlayerForm.h"
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


void UOverlayWidget::CreateFormWheelWidgets_Implementation(
	const TArray<UMVVM_PlayerForms*>& PlayerFormsViewModels,
	UPanelWidget* ParentWidget
)
{
	if (!FormWheelWidgetClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] FormWheelWidgetClass is null"), *GetName());
		return;
	}
	for (UMVVM_PlayerForms* PlayerFormsViewModel : PlayerFormsViewModels)
	{
		ParentWidget->AddChild(CreateFormWheelWidget(PlayerFormsViewModel));
	}
}


UFormWheelWidget* UOverlayWidget::CreateFormWheelWidget_Implementation(
	UMVVM_PlayerForms* PlayerFormsViewModel
)
{
	if (UFormWheelWidget* FormWheelWidget = CreateWidget<UFormWheelWidget>(
		this,
		FormWheelWidgetClass,
		PlayerFormsViewModel->CreateWidgetName(FString("FormWheel"))
	))
	{
		FormWheelWidget->BindViewModel(PlayerFormsViewModel);
		FormWheelWidget->Hide(false);
		FormWheelWidgets.Add(FormWheelWidget);
		return FormWheelWidget;
	}
	UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to create FormWheelWidget"), *GetName());
	return nullptr;
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

void UOverlayWidget::BindViewModels_Implementation(
	const TArray<UMVVM_PlayerState*>& ViewModels,
	const TArray<UMVVM_PlayerAbilityStates*>& AbilityViewModels,
	const TArray<UMVVM_PlayerForms*>& FormViewModels
)
{
	// TODO
}

void UOverlayWidget::Hide_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UOverlayWidget::Show_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}
