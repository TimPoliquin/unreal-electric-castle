// Copyright Alien Shores


#include "UI/Widget/Loading/LoadingScreenWidget.h"

void ULoadingScreenWidget::Show_Implementation(const bool bImmediate)
{
	if (!IsInViewport())
	{
		AddToViewport(INT_MAX);
	}
	SetVisibility(ESlateVisibility::Visible);
}

void ULoadingScreenWidget::Hide_Implementation(const bool bImmediate)
{
	SetVisibility(ESlateVisibility::Hidden);
	RemoveFromParent();
}
