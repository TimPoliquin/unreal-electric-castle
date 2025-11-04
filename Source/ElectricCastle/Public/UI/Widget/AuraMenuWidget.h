// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraMenuWidget.generated.h"

class UButton;
class UAbilitySystemComponent;
class UAttributeSet;
class UAuraInventoryWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuraMenuWidgetClosedSignature);

UENUM(BlueprintType)
enum class EAuraMenuTab : uint8
{
	Attributes,
	Spells,
	Inventory,
	System
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAuraMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UAuraMenuWidget(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintNativeEvent)
	void InitializeDependencies(
		AActor* InPlayer
	);
	UFUNCTION(BlueprintCallable)
	void InitializeTabButton(EAuraMenuTab Tab, UButton* Button);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OpenMenu(const EAuraMenuTab& OpenTab);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CloseMenu();
	FOnAuraMenuWidgetClosedSignature OnAuraMenuClosed;

protected:
	/** Config **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu")
	TMap<EAuraMenuTab, FString> MenuTabNames;
	/** Runtime **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu")
	EAuraMenuTab ActiveMenuTab;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnActiveTabChanged(const EAuraMenuTab& Tab);
	UFUNCTION(BlueprintCallable)
	void ChangeActiveTab(const EAuraMenuTab& Tab);
	UFUNCTION(BlueprintCallable)
	void ActivateAttributesTab();
	UFUNCTION(BlueprintCallable)
	void ActivateSpellsTab();
	UFUNCTION(BlueprintCallable)
	void ActivateInventoryTab();
	UFUNCTION(BlueprintCallable)
	void ActivateSystemTab();
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeAttributesTabDependencies(
		AActor* InPlayer
	);
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeSpellsTabDependencies(
		AActor* InPlayer
	);
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeInventoryTabDependencies(
		AActor* InPlayer
	);
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeSystemTabDependencies(
		AActor* InPlayer
	);
};
