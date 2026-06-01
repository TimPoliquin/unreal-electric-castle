// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Actor/Status/StatusEffectTypes.h"
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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UStatusEffectWidget* AddStatusEffectWidget(const FGameplayTag& StatusEffectTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveStatusEffectWidget(const FGameplayTag& StatusEffectTag);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	TSubclassOf<UStatusEffectWidget> StatusEffectWidgetClass;

private:
	UStatusEffectWidget* CreateStatusEffectWidget(const FGameplayTag& StatusEffectTag);
	UPROPERTY()
	TMap<FGameplayTag, UStatusEffectWidget*> StatusEffectWidgets;
};
