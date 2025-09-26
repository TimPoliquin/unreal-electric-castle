// Copyright Alien Shores


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Progression/ProgressionComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	check(AttributeInfo);
	if (UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		ProgressionComponent->OnAttributePointsChangeDelegate.AddUniqueDynamic(
			this,
			&UAttributeMenuWidgetController::OnAttributePointsChanged
		);
	}
	if (!GetAttributeSet())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No attribute set!"), *GetName())
		return;
	}
	for (auto& Pair : GetAttributeSet()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	check(AttributeInfo);
	if (!GetAttributeSet())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No attribute set provided!"), *GetName())
		return;
	}
	for (auto& Pair : GetAttributeSet()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	if (const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(GetPlayerState()))
	{
		OnAttributePointsChanged(
			FAuraIntAttributeChangedPayload::CreateBroadcastPayload(FElectricCastleGameplayTags::Get().Attributes_Progression_AttributePoints, ProgressionComponent->GetAttributePoints()));
	}
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAbilitySystemComponent()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(
	const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute
) const
{
	FAttributeInfoRow Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::OnAttributePointsChanged(const FAuraIntAttributeChangedPayload& Payload)
{
	OnAttributePointsChangedDelegate.Broadcast(Payload);
}
