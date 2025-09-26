// Copyright Alien Shores


#include "UI/WidgetController/ElectricCastleWidgetController.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Character/ElectricCastlePlayerCharacter.h"

void UElectricCastleWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	Player = WidgetControllerParams.Player;
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UElectricCastleWidgetController::BroadcastAbilityInfo()
{
	if (!GetAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		return;
	}
	if (!AbilityInfo)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("Ability Info needs to be set in Widget Controller [%s]"), *GetName());
	}
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec)
		{
			// Get ability tag for given ability spec.
			const FGameplayTag AbilityTag = UElectricCastleAbilitySystemLibrary::GetAbilityTagFromSpec(AbilitySpec);
			FElectricCastleAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.InputTag = UElectricCastleAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = UElectricCastleAbilitySystemLibrary::GetStatusTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);
	GetAbilitySystemComponent()->ForEachAbility(BroadcastDelegate);
}

void UElectricCastleWidgetController::UnbindAll_Implementation(const UObject* BoundObject)
{
	AbilityInfoDelegate.RemoveAll(BoundObject);
}

AElectricCastlePlayerCharacter* UElectricCastleWidgetController::GetAuraCharacter()
{
	if (AuraCharacter)
	{
		return AuraCharacter;
	}
	AuraCharacter = Cast<AElectricCastlePlayerCharacter>(Player);
	return AuraCharacter;
}

AElectricCastlePlayerController* UElectricCastleWidgetController::GetAuraPlayerController()
{
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = Cast<AElectricCastlePlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

AElectricCastlePlayerState* UElectricCastleWidgetController::GetPlayerState()
{
	return PlayerState;
}

UElectricCastleAbilitySystemComponent* UElectricCastleWidgetController::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}

UElectricCastleAttributeSet* UElectricCastleWidgetController::GetAttributeSet()
{
	return AttributeSet;
}
