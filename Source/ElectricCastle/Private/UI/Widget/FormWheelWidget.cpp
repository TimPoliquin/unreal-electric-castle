// Copyright Alien Shores


#include "UI/Widget/FormWheelWidget.h"

#include "UI/ViewModel/Form/MVVM_PlayerForms.h"

void UFormWheelWidget::Show_Implementation()
{
}

void UFormWheelWidget::Hide_Implementation()
{
}

void UFormWheelWidget::BindViewModel_Implementation(UMVVM_PlayerForms* InPlayerFormsViewModel)
{
	PlayerFormsViewModel = InPlayerFormsViewModel;
}

int32 UFormWheelWidget::GetPlayerIndex() const
{
	if (PlayerFormsViewModel)
	{
		return PlayerFormsViewModel->GetPlayerIndex();
	}
	return -1;
}
