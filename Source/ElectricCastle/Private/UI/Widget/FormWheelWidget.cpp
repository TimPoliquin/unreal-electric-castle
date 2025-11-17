// Copyright Alien Shores


#include "UI/Widget/FormWheelWidget.h"

#include "Components/PanelWidget.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "UI/ViewModel/Form/MVVM_PlayerForm.h"
#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "UI/Widget/Form/FormWheelFormWidget.h"
#include "UI/Widget/Layout/RadialLayout.h"

void UFormWheelWidget::Show_Implementation(const bool bAnimate)
{
	SetVisibility(ESlateVisibility::Visible);
	if (URadialLayout* RadialLayout = Cast<URadialLayout>(GetFormsContainer()))
	{
		RadialLayout->SetSelectedIndex(GetSelectedIndex());
		RadialLayout->InitializeForInput(true);
	}
}

void UFormWheelWidget::Hide_Implementation(const bool bAnimate)
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UFormWheelWidget::BindViewModel_Implementation(UMVVM_PlayerForms* InPlayerFormsViewModel)
{
	PlayerFormsViewModel = InPlayerFormsViewModel;
	CreateFormWidgets(InPlayerFormsViewModel);
	PlayerFormsViewModel->OnVisibilityChange.AddUniqueDynamic(this, &UFormWheelWidget::OnFormWheelVisibilityChange);
	PlayerFormsViewModel->OnAvailableFormsChangedDelegate.AddUniqueDynamic(this, &UFormWheelWidget::OnAvailableFormsChanged);
}

int32 UFormWheelWidget::GetPlayerIndex() const
{
	if (PlayerFormsViewModel)
	{
		return PlayerFormsViewModel->GetPlayerIndex();
	}
	return -1;
}

UPanelWidget* UFormWheelWidget::GetFormsContainer_Implementation() const
{
	return Cast<UPanelWidget>(GetRootWidget());
}

void UFormWheelWidget::CreateFormWidgets_Implementation(UMVVM_PlayerForms* PlayerForms)
{
	UPanelWidget* Container = GetFormsContainer();
	if (!Container)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Could not find forms container"), *GetName());
		return;
	}
	for (UMVVM_PlayerForm* FormViewModel : PlayerForms->GetPlayerFormViewModels())
	{
		UFormWheelFormWidget* FormWidget = CreateWidget<UFormWheelFormWidget>(this, FormWidgetClass, FName(FString::Printf(TEXT("FormWidget_%s"), *FormViewModel->GetFormTag().ToString())));
		FormWidget->BindViewModel(FormViewModel);
		if (FormViewModel->GetIsAvailable())
		{
			Container->AddChild(FormWidget);
		}
		FormWidgets.Add(FormWidget);
	}
}

int32 UFormWheelWidget::GetSelectedIndex() const
{
	for (int32 Index = 0; Index < FormWidgets.Num(); ++Index)
	{
		if (FormWidgets[Index]->IsFormSelected())
		{
			return Index;
		}
	}
	return 0;
}

void UFormWheelWidget::OnFormWheelVisibilityChange(const bool bIsVisible)
{
	if (bIsVisible)
	{
		Show();
	}
	else
	{
		Hide();
	}
}

void UFormWheelWidget::OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload)
{
	UPanelWidget* Container = GetFormsContainer();
	if (!Container)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Could not find forms container"), *GetName());
		return;
	}
	for (UFormWheelFormWidget* FormWidget : FormWidgets)
	{
		if (Payload.AvailableForms.HasTagExact(FormWidget->GetFormTag()))
		{
			if (!Container->HasChild(FormWidget))
			{
				Container->AddChild(FormWidget);
			}
		}
		else
		{
			Container->RemoveChild(FormWidget);
		}
	}
}
