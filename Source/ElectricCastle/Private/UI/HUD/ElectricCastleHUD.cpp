// Copyright Alien Shores


#include "UI/HUD/ElectricCastleHUD.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_Inventory.h"
#include "UI/Widget/AuraMenuWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/Widget/AuraOverlayWidget.h"

void AElectricCastleHUD::BeginPlay()
{
	Super::BeginPlay();
	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
	}
	if (!AttributeMenuWidgetController)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
	}
	if (!SpellMenuWidgetController)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
	}
}


UOverlayWidgetController* AElectricCastleHUD::GetOverlayWidgetController() const
{
	return OverlayWidgetController;
}

void AElectricCastleHUD::InitializeWidgets(
	AActor* InPlayer,
	APlayerController* InPlayerController,
	AElectricCastlePlayerState* InPlayerState,
	UElectricCastleAbilitySystemComponent* InAbilitySystemComponent,
	UElectricCastleAttributeSet* InAttributeSet
)
{
	if (bInitialized)
	{
		return;
	}
	OverlayWidget = CreateAuraWidget(
		OverlayWidgetClass,
		OverlayWidgetControllerClass,
		InPlayer,
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
	OverlayWidget->OnOpenMenuDelegate.AddDynamic(this, &AElectricCastleHUD::OpenMenu);
	InitializeInventoryViewModel();
	MenuWidget = CreateWidget<UAuraMenuWidget>(GetWorld(), MenuWidgetClass, FName("MenuWidget"));
	MenuWidget->InitializeDependencies(
		GetOwningPawn()
	);
	MenuWidget->OnAuraMenuClosed.AddDynamic(this, &AElectricCastleHUD::OnMenuClosed);
	MenuWidget->SetVisibility(ESlateVisibility::Hidden);
	MenuWidget->AddToViewport();
	auto OnAbilitySystemReadyCallback = [&]()
	{
		// const FWidgetControllerParams Params = FWidgetControllerParams(InPlayer, InPlayerController, InPlayerState, InAbilitySystemComponent, InAttributeSet);
		// InitializeOverlayWidgetController(Params);
		// InitializeAttributeWidgetController(Params);
		// InitializeSpellMenuWidgetController(Params);
	};
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(InAbilitySystemComponent))
	{
		if (AbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
		{
			OnAbilitySystemReadyCallback();
		}
		else
		{
			AbilitySystemComponent->OnAbilitiesGivenDelegate.AddLambda(OnAbilitySystemReadyCallback);
		}
	}
	bInitialized = true;
}

UAttributeMenuWidgetController* AElectricCastleHUD::GetAttributeMenuWidgetController() const
{
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AElectricCastleHUD::GetSpellMenuWidgetController() const
{
	return SpellMenuWidgetController;
}

UMVVM_Inventory* AElectricCastleHUD::GetInventoryViewModel()
{
	return InventoryViewModel;
}

void AElectricCastleHUD::OpenMenu(const EAuraMenuTab& OpenTab)
{
	GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());
	OverlayWidget->Hide();
	MenuWidget->OpenMenu(OpenTab);
}

void AElectricCastleHUD::OnMenuClosed()
{
	OverlayWidget->Show();
	GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
}

UAuraOverlayWidget* AElectricCastleHUD::CreateAuraWidget(
	const TSubclassOf<UAuraOverlayWidget>& WidgetClass,
	const TSubclassOf<UElectricCastleWidgetController>& WidgetControllerClass,
	AActor* InOwner,
	APlayerController* InPlayerController,
	AElectricCastlePlayerState* InPlayerState,
	UElectricCastleAbilitySystemComponent* InAbilitySystemComponent,
	UElectricCastleAttributeSet* InAttributeSet
)
{
	checkf(WidgetClass, TEXT("Widget class uninitialized; please fill out BP_ElectricCastleHUD"));
	checkf(
		WidgetControllerClass,
		TEXT("Widget controller class uninitialized; please fill out BP_ElectricCastleHUD")
	);

	UAuraOverlayWidget* Widget = CreateWidget<UAuraOverlayWidget>(GetWorld(), WidgetClass);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController();
	Widget->SetWidgetController(WidgetController);
	Widget->AddToViewport();
	return Widget;
}

void AElectricCastleHUD::InitializeInventoryViewModel()
{
	InventoryViewModel = NewObject<UMVVM_Inventory>(this, InventoryViewModelClass);
	InventoryViewModel->InitializeInventoryItems();
	InventoryViewModel->InitializeDependencies();
}

void AElectricCastleHUD::InitializeOverlayWidgetController(const FWidgetControllerParams& Params)
{
	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
	}
	OverlayWidgetController->SetWidgetControllerParams(Params);
	OverlayWidgetController->BindCallbacksToDependencies();
	OverlayWidgetController->BroadcastInitialValues();
}

void AElectricCastleHUD::InitializeAttributeWidgetController(const FWidgetControllerParams& Params)
{
	if (!AttributeMenuWidgetController)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
	}
	AttributeMenuWidgetController->SetWidgetControllerParams(Params);
	AttributeMenuWidgetController->BindCallbacksToDependencies();
	AttributeMenuWidgetController->BroadcastInitialValues();
}

void AElectricCastleHUD::InitializeSpellMenuWidgetController(const FWidgetControllerParams& Params)
{
	if (!SpellMenuWidgetController)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
	}
	SpellMenuWidgetController->SetWidgetControllerParams(Params);
	SpellMenuWidgetController->BindCallbacksToDependencies();
	SpellMenuWidgetController->BroadcastInitialValues();
}
