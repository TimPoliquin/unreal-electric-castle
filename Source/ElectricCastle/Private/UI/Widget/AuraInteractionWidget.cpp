// Copyright Alien Shores


#include "UI/Widget/AuraInteractionWidget.h"

#include "Components/TextBlock.h"

void UAuraInteractionWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	InitializeActionText();
	InitializeKeyText();
}

void UAuraInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeActionText();
	InitializeKeyText();
	if (bStartHidden)
	{
		InitializeHiddenState();
	}
}

void UAuraInteractionWidget::InitializeShownState_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UAuraInteractionWidget::InitializeHiddenState_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UAuraInteractionWidget::SetInteractionKey_Implementation(const FString& Key)
{
	KeyText = Key;
	InitializeKeyText();
}

void UAuraInteractionWidget::SetActionText_Implementation(const FString& Key)
{
	ActionText = Key;
	InitializeActionText();
}

void UAuraInteractionWidget::SetStartHidden_Implementation(const bool bInStartHidden)
{
	bStartHidden = bInStartHidden;
}

void UAuraInteractionWidget::SetAutoDestroyOnHide_Implementation(const bool bInAutoDestroyOnHide)
{
	bAutoDestroyOnHide = bInAutoDestroyOnHide;
}

void UAuraInteractionWidget::Show_Implementation()
{
	if (IsVisible())
	{
		return;
	}
	InitializeShownState();
}

void UAuraInteractionWidget::Shown_Implementation()
{
	OnShown.Broadcast(GetOwningPlayerPawn());
}

void UAuraInteractionWidget::Hidden_Implementation()
{
	OnHidden.Broadcast(GetOwningPlayerPawn());
	if (bAutoDestroyOnHide)
	{
		RemoveFromParent();
	}
	else
	{
		InitializeHiddenState();
	}
}

void UAuraInteractionWidget::Hide_Implementation()
{
	// nothing to do here right now
}

void UAuraInteractionWidget::SetDisplayText(const FString DisplayText, UTextBlock* TextWidget)
{
	if (!DisplayText.IsEmpty() && IsValid(TextWidget))
	{
		TextWidget->SetText(FText::FromString(DisplayText));
	}
}
