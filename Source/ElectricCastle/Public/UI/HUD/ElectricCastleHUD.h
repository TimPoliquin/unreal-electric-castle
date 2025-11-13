// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/ElectricCastleWidgetController.h"
#include "ElectricCastleHUD.generated.h"

class UMVVM_PlayerAbilityStates;
class UOverlayWidget;
class UMVVM_PlayerState;
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
UCLASS(Abstract)
class ELECTRICCASTLE_API AElectricCastleHUD : public AHUD
{
	GENERATED_BODY()

public:
	void Initialize();
	void InitializeWidgets(
		AActor* InPlayer,
		APlayerController* InPlayerController,
		AElectricCastlePlayerState* InPlayerState,
		UElectricCastleAbilitySystemComponent* InAbilitySystemComponent,
		UElectricCastleAttributeSet* InAttributeSet
	);

	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(
	) const;
	USpellMenuWidgetController* GetSpellMenuWidgetController(
	) const;
	UFUNCTION(BlueprintCallable)
	UMVVM_Inventory* GetInventoryViewModel();
	UFUNCTION(BlueprintCallable)
	TArray<UMVVM_PlayerState*> GetPlayerStateViewModels() const;
	UFUNCTION(BlueprintCallable)
	TArray<UMVVM_PlayerAbilityStates*> GetPlayerAbilityStatesViewModels() const;


	UFUNCTION(BlueprintCallable)
	void OpenMenu(const EAuraMenuTab& OpenTab);
	UFUNCTION(BlueprintCallable)
	void OnMenuClosed();

protected:
	virtual void BeginPlay() override;

	/** Player State View Model **/
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<UMVVM_PlayerState> PlayerStateViewModelClass;
	UPROPERTY()
	TArray<TObjectPtr<UMVVM_PlayerState>> PlayerStateViewModels;
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<UMVVM_PlayerAbilityStates> PlayerAbilityStatesViewModelClass;
	UPROPERTY()
	TArray<TObjectPtr<UMVVM_PlayerAbilityStates>> PlayerAbilityStatesViewModels;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<UOverlayWidget> OverlayWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOverlayWidget> OverlayWidget;

	/** Menu Widget **/
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UAuraMenuWidget> MenuWidgetClass;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UAuraMenuWidget> MenuWidget;

private:
	/** Attribute Menu Controller **/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

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

	void InitializePlayerStateViewModels();
	void InitializeInventoryViewModel();

	void InitializeOverlayWidget();


	bool bInitialized = false;

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
	if (WidgetController)
	{
		WidgetController->SetWidgetControllerParams(WidgetControllerParams);
		WidgetController->BindCallbacksToDependencies();
	}
	return WidgetController;
}
