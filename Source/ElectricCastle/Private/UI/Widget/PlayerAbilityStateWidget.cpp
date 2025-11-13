// Copyright Alien Shores


#include "UI/Widget/PlayerAbilityStateWidget.h"

#include "UI/ViewModel/MVVM_AbilityState.h"

void UPlayerAbilityStateWidget::BindViewModel_Implementation(UMVVM_AbilityState* InViewModel)
{
	ViewModel = InViewModel;
}

void UPlayerAbilityStateWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (ViewModel)
	{
		ViewModel->UpdateCooldown(InDeltaTime);
	}
}
