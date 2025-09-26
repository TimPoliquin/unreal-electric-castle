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
}


UOverlayWidgetController* AElectricCastleHUD::GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
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
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(InAbilitySystemComponent))
	{
		const FWidgetControllerParams Params = FWidgetControllerParams(InPlayer, InPlayerController, InPlayerState, InAbilitySystemComponent, InAttributeSet);
		if (AbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
		{
			GetOverlayWidgetController(Params)->BroadcastInitialValues();
			GetAttributeMenuWidgetController(Params)->BroadcastInitialValues();
			GetSpellMenuWidgetController(Params)->BroadcastInitialValues();
		}
		else
		{
			AbilitySystemComponent->OnAbilitiesGivenDelegate.AddLambda([this, Params]()
			{
				GetOverlayWidgetController(Params)->BroadcastInitialValues();
				GetAttributeMenuWidgetController(Params)->BroadcastInitialValues();
			});
		}
	}
	bInitialized = true;
}

UAttributeMenuWidgetController* AElectricCastleHUD::GetAttributeMenuWidgetController(
	const FWidgetControllerParams& WidgetControllerParams
)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = InitializeWidgetController<UAttributeMenuWidgetController>(
			AttributeMenuWidgetControllerClass,
			WidgetControllerParams
		);
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AElectricCastleHUD::GetSpellMenuWidgetController(
	const FWidgetControllerParams& WidgetControllerParams
)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = InitializeWidgetController<USpellMenuWidgetController>(
			SpellMenuWidgetControllerClass,
			WidgetControllerParams
		);
	}
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

	const FWidgetControllerParams WidgetControllerParams(
		InOwner,
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
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
