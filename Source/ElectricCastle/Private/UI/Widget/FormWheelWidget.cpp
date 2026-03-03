// Copyright Alien Shores


#include "UI/Widget/Form/FormWheelWidget.h"

#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/SelectionWheel/SelectionWheelManagerComponent.h"
#include "UI/ViewModel/Form/MVVM_PlayerForm.h"
#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "UI/Widget/Form/FormWheelFormWidget.h"
#include "UI/Widget/Layout/RadialLayout.h"
#include "UI/Widget/Layout/RadialLayoutCursor.h"

void UFormWheelWidget::Show_Implementation(const bool bAnimate)
{
	if (USelectionWheelManagerComponent* SelectionWheelManager = ISelectionWheelManagerActorInterface::GetSelectionWheelManagerComponent(PlayerFormsViewModel))
	{
		SelectionWheelManager->AddListener(this);
	}
	if (URadialLayout* RadialLayout = GetFormsContainer())
	{
		RadialLayout->SetSelectedIndex(GetSelectedIndex());
	}
	SetVisibility(ESlateVisibility::Visible);
}

void UFormWheelWidget::Hide_Implementation(const bool bAnimate)
{
	SetVisibility(ESlateVisibility::Hidden);
	if (USelectionWheelManagerComponent* SelectionWheelManager = ISelectionWheelManagerActorInterface::GetSelectionWheelManagerComponent(PlayerFormsViewModel))
	{
		SelectionWheelManager->RemoveListener(this);
	}
}

void UFormWheelWidget::BindViewModel_Implementation(UMVVM_PlayerForms* InPlayerFormsViewModel)
{
	PlayerFormsViewModel = InPlayerFormsViewModel;
	CreateFormWidgets(InPlayerFormsViewModel);
	PlayerFormsViewModel->OnAvailableFormsChangedDelegate.AddUniqueDynamic(this, &UFormWheelWidget::OnAvailableFormsChanged);
}

void UFormWheelWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (USelectionWheelManagerComponent* SelectionWheelManager = ISelectionWheelManagerActorInterface::GetSelectionWheelManagerComponent(PlayerFormsViewModel))
	{
		SelectionWheelManager->RemoveListener(this);
	}
}

int32 UFormWheelWidget::GetPlayerIndex() const
{
	if (PlayerFormsViewModel)
	{
		return PlayerFormsViewModel->GetPlayerIndex();
	}
	return -1;
}

void UFormWheelWidget::OnSelectionWheelAngleChange_Implementation(const float Value)
{
	if (URadialLayout* RadialLayout = GetFormsContainer())
	{
		OnSelectionWheelAngleChange(RadialLayout, Value);
	}
	if (URadialLayoutCursor* LocalCursor = GetCursorWidget())
	{
		OnSelectionWheelAngleChange(LocalCursor, Value);
	}
}

void UFormWheelWidget::OnSelectionWheelConfirm_Implementation()
{
	CommitSelection();
}

void UFormWheelWidget::CommitSelection_Implementation()
{
	if (const URadialLayout* RadialLayout = GetFormsContainer())
	{
		const UFormWheelFormWidget* FormWidget = Cast<UFormWheelFormWidget>(RadialLayout->GetSelectedChild());
		if (!FormWidget)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Unexpected selected form widget"), *GetName());
			return;
		}
		UE_LOG(LogElectricCastle, Log, TEXT("[%s] Selected form: %s"), *GetName(), *FormWidget->GetFormTag().ToString());
		PlayerFormsViewModel->ChangeForm(FormWidget->GetFormTag());
	}
}

URadialLayout* UFormWheelWidget::GetFormsContainer_Implementation() const
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] GetFormsContainer_Implementation is not implemented"), *GetName());
	return nullptr;
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

URadialLayoutCursor* UFormWheelWidget::GetCursorWidget_Implementation() const
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] GetCursorWidget_Implementation is not implemented"), *GetName());
	return nullptr;
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
