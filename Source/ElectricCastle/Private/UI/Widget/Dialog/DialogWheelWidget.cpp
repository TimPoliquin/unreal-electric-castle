// Copyright Alien Shores


#include "UI/Widget/Dialog/DialogWheelWidget.h"

#include "GameplayTagsManager.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/SelectionWheel/SelectionWheelManagerActorInterface.h"
#include "Player/SelectionWheel/SelectionWheelManagerComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/Widget/Dialog/DialogChoiceWidget.h"
#include "UI/Widget/Layout/RadialLayout.h"
#include "UI/Widget/Layout/RadialLayoutCursor.h"

class USelectionWheelManagerComponent;

void UDialogWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	AllDialogChoiceTags = TagsManager.RequestGameplayTagChildren(FElectricCastleGameplayTags::Get().InputTag_Dialog);
}

void UDialogWheelWidget::NativeDestruct()
{
	UnregisterSelectionWheelListener();
	Super::NativeDestruct();
}

void UDialogWheelWidget::Show_Implementation(const bool bAnimate)
{
	SetVisibility(ESlateVisibility::Visible);
	if (USelectionWheelManagerComponent* SelectionWheelManager = ISelectionWheelManagerActorInterface::GetSelectionWheelManagerComponent(GetOwningPlayer()))
	{
		SelectionWheelManager->AddListener(this);
	}
	for (const FGameplayTag& DialogChoiceTag : AllDialogChoiceTags)
	{
		if (UDialogChoiceWidget* DialogChoiceWidget = GetDialogChoiceWidget(DialogChoiceTag))
		{
			DialogChoiceWidget->SetVisibility(DialogChoiceWidget->IsConfigured() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
	}
}

void UDialogWheelWidget::Hide_Implementation(const bool bAnimate)
{
	SetVisibility(ESlateVisibility::Hidden);
	UnregisterSelectionWheelListener();
}

void UDialogWheelWidget::AddDialogChoice_Implementation(const FDialogChoice& InDialogChoice)
{
	if (UDialogChoiceWidget* DialogChoiceWidget = GetDialogChoiceWidget(InDialogChoice.ChoiceTag))
	{
		DialogChoiceWidget->SetDialogChoice(InDialogChoice);
		DialogChoiceWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UDialogWheelWidget::OnSelectionWheelAngleChange_Implementation(const float Value)
{
	if (URadialLayout* RadialLayout = GetRadialLayout())
	{
		OnSelectionWheelAngleChange(RadialLayout, Value);
	}
	if (URadialLayoutCursor* LocalCursor = GetCursorWidget())
	{
		OnSelectionWheelAngleChange(LocalCursor, Value);
	}
}

void UDialogWheelWidget::OnSelectionWheelConfirm_Implementation()
{
	ConfirmSelection();
}


UDialogChoiceWidget* UDialogWheelWidget::GetDialogChoiceWidget_Implementation(const FGameplayTag& DialogChoiceTag) const
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] GetDialogChoiceWidget_Implementation is not implemented"), *GetName());
	return nullptr;
}

URadialLayout* UDialogWheelWidget::GetRadialLayout_Implementation() const
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] GetRadialLayout_Implementation is not implemented"), *GetName());
	return nullptr;
}

URadialLayoutCursor* UDialogWheelWidget::GetCursorWidget_Implementation() const
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] GetCursorWidget_Implementation is not implemented"), *GetName());
	return nullptr;
}

void UDialogWheelWidget::ConfirmSelection_Implementation()
{
	if (const URadialLayout* RadialLayout = GetRadialLayout())
	{
		if (const UDialogChoiceWidget* SelectedChoice = Cast<UDialogChoiceWidget>(RadialLayout->GetSelectedChild()))
		{
			OnDialogChoiceConfirmed.Broadcast(FDialogChoiceConfirmedPayload(SelectedChoice->GetDialogChoiceTag()));
		}
	}
}

void UDialogWheelWidget::UnregisterSelectionWheelListener() const
{
	if (USelectionWheelManagerComponent* SelectionWheelManager = ISelectionWheelManagerActorInterface::GetSelectionWheelManagerComponent(GetOwningPlayer()))
	{
		SelectionWheelManager->RemoveListener(this);
	}
}
