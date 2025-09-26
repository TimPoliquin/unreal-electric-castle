// Copyright Alien Shores


#include "UI/Widget/AuraFishingMessageWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "UI/Library/WidgetFunctionLibrary.h"

FReply UAuraFishingMessageWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == CancelKey)
	{
		Cancel();
		return UWidgetBlueprintLibrary::Handled().NativeReply;
	}
	if (InKeyEvent.GetKey() == ContinueKey)
	{
		Continue();
		return UWidgetBlueprintLibrary::Handled().NativeReply;
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UAuraFishingMessageWidget::Cancel_Implementation()
{
	Hide();
	OnCancel.Broadcast(GetOwningPlayerPawn());
}

void UAuraFishingMessageWidget::Continue_Implementation()
{
	Hide();
	OnContinue.Broadcast(GetOwningPlayerPawn());
}

void UAuraFishingMessageWidget::Show_Implementation()
{
	if (bIsShowing)
	{
		return;
	}
	bIsShowing = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayer(), this);
}

void UAuraFishingMessageWidget::Hide_Implementation()
{
	if (!bIsShowing)
	{
		return;
	}
	bIsShowing = false;
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), nullptr);
}

void UAuraFishingMessageWidget::Shown_Implementation()
{
	bAllowInput = true;
	OnShown.Broadcast(GetOwningPlayerPawn());
}

void UAuraFishingMessageWidget::Hidden_Implementation()
{
	OnHidden.Broadcast(GetOwningPlayerPawn());
	RemoveFromParent();
}

bool UAuraFishingMessageWidget::IsKeyBoundToInputAction(const FKeyEvent& Input, const UInputAction* InputAction) const
{
	if (!bAllowInput)
	{
		return false;
	}
	return UWidgetFunctionLibrary::IsInputActionBoundToKey(GetOwningPlayer(), UKismetInputLibrary::GetUserIndex(Input), InputAction, Input.GetKey());
}
