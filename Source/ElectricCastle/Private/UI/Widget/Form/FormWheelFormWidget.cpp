// Copyright Alien Shores


#include "UI/Widget/Form/FormWheelFormWidget.h"

#include "GameplayTagContainer.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "UI/ViewModel/Form/MVVM_PlayerForm.h"

FGameplayTag UFormWheelFormWidget::GetFormTag_Implementation() const
{
	return FGameplayTag::EmptyTag;
}

void UFormWheelFormWidget::BindViewModel_Implementation(UMVVM_PlayerForm* InViewModel)
{
	PlayerFormViewModel = InViewModel;
}

void UFormWheelFormWidget::Hover_Implementation()
{
}

void UFormWheelFormWidget::Unhover_Implementation()
{
}

void UFormWheelFormWidget::Select_Implementation()
{
}

bool UFormWheelFormWidget::IsFormSelected() const
{
	if (!PlayerFormViewModel)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerFormViewModel is null"), *GetName());
		return false;
	}
	return PlayerFormViewModel->GetIsSelected();
}
