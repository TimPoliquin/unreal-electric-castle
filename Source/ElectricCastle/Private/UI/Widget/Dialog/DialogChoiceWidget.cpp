// Copyright Alien Shores


#include "UI/Widget/Dialog/DialogChoiceWidget.h"

void UDialogChoiceWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetDialogChoiceRaw(DialogChoiceTag, Text, Icon);
}

FGameplayTag UDialogChoiceWidget::GetDialogChoiceTag() const
{
	return DialogChoiceTag;
}

void UDialogChoiceWidget::Hover_Implementation()
{
	SetRenderScale(FVector2D(1.5f, 1.5f));
}

void UDialogChoiceWidget::Unhover_Implementation()
{
	SetRenderScale(FVector2D(1.f, 1.f));
}

bool UDialogChoiceWidget::IsConfigured_Implementation() const
{
	return DialogChoiceTag.IsValid() && !Text.IsEmpty();
}

void UDialogChoiceWidget::SetDialogChoice_Implementation(const FDialogChoice& DialogChoice)
{
	SetDialogChoiceRaw(DialogChoice.ChoiceTag, DialogChoice.Text, DialogChoice.Icon);
}

void UDialogChoiceWidget::SetDialogChoiceRaw_Implementation(const FGameplayTag& InDialogChoiceTag, const FText& InText, UTexture2D* InIcon)
{
	DialogChoiceTag = InDialogChoiceTag;
	SetChoiceText(InText);
	SetChoiceIcon(InIcon);
}

void UDialogChoiceWidget::SetChoiceIcon_Implementation(UTexture2D* InChoiceIcon)
{
	Icon = InChoiceIcon;
}

void UDialogChoiceWidget::SetChoiceText_Implementation(const FText& InChoiceText)
{
	Text = InChoiceText;
}
