// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Status/StatusEffectTypes.h"
#include "StatusEffectBarWidget.generated.h"

class UStatusEffectWidget;
class UStatusEffectManagerComponent;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UStatusEffectBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindDependencies(UStatusEffectManagerComponent* StatusEffectManager);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnStatusEffectAdded(const FOnStatusEffectAddedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void OnStatusEffectRemoved(const FOnStatusEffectRemovedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void AddStatusEffectWidget(UStatusEffectWidget* StatusEffectWidget);
	UFUNCTION(BlueprintNativeEvent)
	void RemoveStatusEffectWidget(UStatusEffectWidget* StatusEffectWidget);
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	TSubclassOf<UStatusEffectWidget> StatusEffectWidgetClass;
	
private:
	UStatusEffectWidget* CreateStatusEffectWidget(const FGameplayTag& StatusEffectTag);
	UPROPERTY()
	TMap<FGameplayTag, UStatusEffectWidget*> StatusEffectWidgets;
};
