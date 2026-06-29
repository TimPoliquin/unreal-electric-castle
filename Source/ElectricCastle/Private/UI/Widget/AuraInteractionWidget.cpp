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
		VisibilityState = EWidgetVisibilityState::Hidden;
		InitializeHiddenState();
	}
	else
	{
		VisibilityState = EWidgetVisibilityState::Shown;
	}
}

bool UAuraInteractionWidget::CanShow() const
{
	return VisibilityState <= EWidgetVisibilityState::Hiding;
}

bool UAuraInteractionWidget::IsShowing() const
{
	return VisibilityState == EWidgetVisibilityState::Showing;
}

bool UAuraInteractionWidget::CanHide() const
{
	return VisibilityState >= EWidgetVisibilityState::Showing;
}

bool UAuraInteractionWidget::IsHiding() const
{
	return VisibilityState == EWidgetVisibilityState::Hiding;
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

void UAuraInteractionWidget::SetIcon_Implementation(const UTexture2D* Icon)
{
	// nothing to do by default
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
	if (CanShow())
	{
		VisibilityState = EWidgetVisibilityState::Showing;
		InitializeShownState();
		Execute_Show();
	}
}

void UAuraInteractionWidget::Shown_Implementation()
{
	if (VisibilityState == EWidgetVisibilityState::Showing)
	{
		VisibilityState = EWidgetVisibilityState::Shown;
		OnShown.Broadcast(GetOwningPlayerPawn());
	}
}

void UAuraInteractionWidget::Hidden_Implementation()
{
	if (VisibilityState == EWidgetVisibilityState::Hiding)
	{
		VisibilityState = EWidgetVisibilityState::Hidden;
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
}

void UAuraInteractionWidget::Hide_Implementation()
{
	if (CanHide())
	{
		VisibilityState = EWidgetVisibilityState::Hiding;
		Execute_Hide();
	}
}

void UAuraInteractionWidget::SetDisplayText(const FString DisplayText, UTextBlock* TextWidget)
{
	if (IsValid(TextWidget))
	{
		TextWidget->SetText(FText::FromString(DisplayText));
	}
}
