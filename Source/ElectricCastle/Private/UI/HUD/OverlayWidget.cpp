// Copyright Alien Shores


#include "UI/HUD/OverlayWidget.h"
#include "UI/HUD/ElectricCastleHUD.h"

class AElectricCastleHUD;

TArray<UMVVM_PlayerState*> UOverlayWidget::GetPlayerStateViewModels() const
{
	if (const AElectricCastleHUD* HUD = Cast<AElectricCastleHUD>(GetOwningPlayer()->GetHUD()))
	{
		return HUD->GetPlayerStateViewModels();
	}
	return TArray<UMVVM_PlayerState*>();
}

void UOverlayWidget::BindViewModels_Implementation(
	const TArray<UMVVM_PlayerState*>& ViewModels,
	const TArray<UMVVM_PlayerAbilityStates*>& AbilityViewModels
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
