// Copyright Alien Shores


#include "UI/Widget/PlayerAbilityStatesWidget.h"

void UPlayerAbilityStatesWidget::BindViewModel_Implementation(UMVVM_PlayerAbilityStates* ViewModel)
{
	// TODO
}

void UPlayerAbilityStatesWidget::Show_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UPlayerAbilityStatesWidget::Hide_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);
}
