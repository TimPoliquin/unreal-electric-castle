// Copyright Alien Shores


#include "Actor/Status/Plugin/StatusEffectIconPlugin.h"

#include "Actor/Status/StatusEffectActor.h"
#include "Actor/Status/StatusEffectManagerComponent.h"
#include "UI/Widget/Status/StatusEffectBarWidget.h"
#include "UI/Widget/Status/StatusEffectWidget.h"

void UStatusEffectIconPlugin::ApplyEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor, const FStatusEffectDuration& InDuration) const
{
	if (const UStatusEffectManagerComponent* StatusEffectManagerComponent = IStatusEffectActor::GetStatusEffectManagerComponent(InActor))
	{
		if (UStatusEffectBarWidget* BarWidget = StatusEffectManagerComponent->GetStatusEffectBarWidget())
		{
			if (UStatusEffectWidget* Widget = BarWidget->AddStatusEffectWidget(StatusEffectTag))
			{
				Widget->SetIconFromTexture2D(Icon);
				Widget->SetBackgroundColor(EffectColor);
				Widget->SetDuration(InDuration.Duration);
			}
		}
	}
}

void UStatusEffectIconPlugin::RemoveEffect_Implementation(const FGameplayTag& StatusEffectTag, AActor* InActor) const
{
	if (const UStatusEffectManagerComponent* StatusEffectManagerComponent = IStatusEffectActor::GetStatusEffectManagerComponent(InActor))
	{
		if (UStatusEffectBarWidget* BarWidget = StatusEffectManagerComponent->GetStatusEffectBarWidget())
		{
			BarWidget->RemoveStatusEffectWidget(StatusEffectTag);
		}
	}
}
