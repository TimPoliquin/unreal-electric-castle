// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ElectricCastleWidgetController.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
struct FGameplayTag;
class UAttributeInfo;
struct FAttributeInfoRow;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAttributeInfoRow&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ELECTRICCASTLE_API UAttributeMenuWidgetController : public UElectricCastleWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnPlayerStatChangedSignature OnAttributePointsChangedDelegate;

	UFUNCTION(BlueprintCallable, Category = "GAS|Attributes")
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GAS|Attributes")
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
	UFUNCTION()
	void OnAttributePointsChanged(const FAuraIntAttributeChangedPayload& Payload);
};
