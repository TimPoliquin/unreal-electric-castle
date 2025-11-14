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

void UOverlayWidget::InitializeDependencies(AElectricCastlePlayerState* PlayerState) const
{
	if (AElectricCastlePlayerController* PlayerController = Cast<AElectricCastlePlayerController>(
		PlayerState->GetPlayerController()
	))
	{
		if (PlayerController->IsLocalController())
		{
			PlayerController->OnFormWheelVisibilityChange.AddUniqueDynamic(
				this,
				&UOverlayWidget::OnFormWheelVisibilityChange
			);
		}
	}
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
	TArray<UMVVM_PlayerForms*>& PlayerFormsViewModels,
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
	UFormWheelWidget* FormWheelWidget = CreateWidget<UFormWheelWidget>(
		UWidgetFunctionLibrary::GetPlayerController(GetOwningPlayer()),
		FormWheelWidgetClass,
		PlayerFormsViewModel->CreateWidgetName(FString("FormWheel"))
	);
	FormWheelWidget->BindViewModel(PlayerFormsViewModel);
	FormWheelWidgets.Add(FormWheelWidget);
	return FormWheelWidget;
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
