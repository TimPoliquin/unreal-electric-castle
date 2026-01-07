// Copyright Alien Shores


#include "UI/Widget/Status/StatusEffectBarWidget.h"

#include "Character/Status/StatusEffectManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "UI/Widget/Status/StatusEffectWidget.h"

void UStatusEffectBarWidget::BindDependencies_Implementation(UStatusEffectManagerComponent* StatusEffectManager)
{
	StatusEffectManager->OnStatusEffectAddedDelegate.AddUniqueDynamic(this, &UStatusEffectBarWidget::OnStatusEffectAdded);
	StatusEffectManager->OnStatusEffectRemovedDelegate.AddUniqueDynamic(this, &UStatusEffectBarWidget::OnStatusEffectRemoved);
}

void UStatusEffectBarWidget::OnStatusEffectAdded_Implementation(const FOnStatusEffectAddedPayload& Payload)
{
	for (const FGameplayTag& StatusEffectTag : Payload.EffectTags)
	{
		UStatusEffectWidget* Widget = StatusEffectWidgets.Contains(StatusEffectTag) ? StatusEffectWidgets[StatusEffectTag] : CreateStatusEffectWidget(StatusEffectTag);
		if (!Widget)
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to create status effect widget for tag [%s]"), *GetName(), *StatusEffectTag.ToString());
			return;
		}
		Widget->SetDuration(Payload.Duration);
	}
}

void UStatusEffectBarWidget::OnStatusEffectRemoved_Implementation(const FOnStatusEffectRemovedPayload& Payload)
{
	for (const FGameplayTag& StatusEffectTag : Payload.EffectTags)
	{
		if (StatusEffectWidgets.Contains(StatusEffectTag))
		{
			RemoveStatusEffectWidget(StatusEffectWidgets[StatusEffectTag]);
		}
	}
}

void UStatusEffectBarWidget::AddStatusEffectWidget_Implementation(UStatusEffectWidget* StatusEffectWidget)
{
	StatusEffectWidgets.Add(StatusEffectWidget->GetStatusEffectTag(), StatusEffectWidget);
}

void UStatusEffectBarWidget::RemoveStatusEffectWidget_Implementation(UStatusEffectWidget* StatusEffectWidget)
{
	StatusEffectWidgets.Remove(StatusEffectWidget->GetStatusEffectTag());
	StatusEffectWidget->RemoveFromParent();
}

UStatusEffectWidget* UStatusEffectBarWidget::CreateStatusEffectWidget(const FGameplayTag& StatusEffectTag)
{
	if (!StatusEffectWidgetClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Status effect widget class not set!"), *GetName());
		return nullptr;
	}
	UStatusEffectWidget* Widget = CreateWidget<UStatusEffectWidget>(GetWorld(), StatusEffectWidgetClass);
	Widget->SetStatusEffectTag(StatusEffectTag);
	AddStatusEffectWidget(Widget);
	return Widget;
}
