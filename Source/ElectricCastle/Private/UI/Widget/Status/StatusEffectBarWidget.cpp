// Copyright Alien Shores


#include "UI/Widget/Status/StatusEffectBarWidget.h"

#include "Actor/Status/StatusEffectManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "UI/Widget/Status/StatusEffectWidget.h"

void UStatusEffectBarWidget::BindDependencies_Implementation(UStatusEffectManagerComponent* StatusEffectManager)
{
	StatusEffectManager->SetStatusEffectBarWidget(this);
}

UStatusEffectWidget* UStatusEffectBarWidget::AddStatusEffectWidget_Implementation(const FGameplayTag& StatusEffectTag)
{
	return StatusEffectWidgets.Contains(StatusEffectTag) ? StatusEffectWidgets[StatusEffectTag] : CreateStatusEffectWidget(StatusEffectTag);
}

void UStatusEffectBarWidget::RemoveStatusEffectWidget_Implementation(const FGameplayTag& StatusEffectTag)
{
	if (StatusEffectWidgets.Contains(StatusEffectTag))
	{
		if (UStatusEffectWidget* Widget = StatusEffectWidgets[StatusEffectTag])
		{
			Widget->RemoveFromParent();
		}
	}
	StatusEffectWidgets.Remove(StatusEffectTag);
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
	StatusEffectWidgets.Add(StatusEffectTag, Widget);
	return Widget;
}
