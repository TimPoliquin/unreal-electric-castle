// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/ElectricCastleWidgetController.h"
#include "ElectricCastleHUD.generated.h"

class UAuraOverlayWidget;
enum class EAuraMenuTab : uint8;
class UAuraMenuWidget;
class UAuraInventoryWidget;
class UMVVM_Inventory;
class USpellMenuWidgetController;
class UElectricCastleWidgetController;
class UAttributeMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UAuraUserWidget;
class UOverlayWidgetController;
class APlayerController;
class APlayerState;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API AElectricCastleHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitializeWidgets(
		AActor* InPlayer,
		APlayerController* InPlayerController,
		AElectricCastlePlayerState* InPlayerState,
		UElectricCastleAbilitySystemComponent* InAbilitySystemComponent,
		UElectricCastleAttributeSet* InAttributeSet
	);

	UOverlayWidgetController* GetOverlayWidgetController() const;
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(
	) const;
	USpellMenuWidgetController* GetSpellMenuWidgetController(
	) const;
	UFUNCTION(BlueprintCallable)
	UMVVM_Inventory* GetInventoryViewModel();

	UFUNCTION(BlueprintCallable)
	void OpenMenu(const EAuraMenuTab& OpenTab);
	UFUNCTION(BlueprintCallable)
	void OnMenuClosed();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAuraOverlayWidget> OverlayWidget;

	/** Menu Widget **/
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UAuraMenuWidget> MenuWidgetClass;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UAuraMenuWidget> MenuWidget;

private:
	/** Overlay Widget Controller **/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraOverlayWidget> OverlayWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	/** Attribute Menu Controller **/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UAuraOverlayWidget* CreateAuraWidget(
		const TSubclassOf<UAuraOverlayWidget>& WidgetClass,
		const TSubclassOf<UElectricCastleWidgetController>& WidgetControllerClass,
		AActor* InOwner,
		APlayerController* InPlayerController,
		AElectricCastlePlayerState* InPlayerState,
		UElectricCastleAbilitySystemComponent* InAbilitySystemComponent,
		UElectricCastleAttributeSet* InAttributeSet
	);

	/** Spell Menu Widget Controller */
	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	template <typename T>
	T* InitializeWidgetController(
		const TSubclassOf<T>& WidgetControllerClass,
		const FWidgetControllerParams& WidgetControllerParams
	);

	/** Inventory View Model */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UMVVM_Inventory> InventoryViewModelClass;
	UPROPERTY()
	TObjectPtr<UMVVM_Inventory> InventoryViewModel;
	void InitializeInventoryViewModel();

	bool bInitialized = false;

	void InitializeOverlayWidgetController(const FWidgetControllerParams& Params);
	void InitializeAttributeWidgetController(const FWidgetControllerParams& Params);
	void InitializeSpellMenuWidgetController(const FWidgetControllerParams& Params);
};

template <typename T>
T* AElectricCastleHUD::InitializeWidgetController(
	const TSubclassOf<T>& WidgetControllerClass,
	const FWidgetControllerParams& WidgetControllerParams
)
{
	T* WidgetController = Cast<T>(
		NewObject<UElectricCastleWidgetController>(
			this,
			WidgetControllerClass
		)
	);
	WidgetController->SetWidgetControllerParams(WidgetControllerParams);
	WidgetController->BindCallbacksToDependencies();
	return WidgetController;
}
