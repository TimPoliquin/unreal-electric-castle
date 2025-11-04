// Copyright Alien Shores


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	check(AttributeInfo);
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
