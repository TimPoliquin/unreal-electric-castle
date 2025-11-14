// Copyright Alien Shores


#include "UI/HUD/ElectricCastleHUD.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/GameStateBase.h"
#include "Player/ElectricCastlePlayerState.h"
#include "UI/HUD/OverlayWidget.h"
#include "UI/ViewModel/MVVM_Inventory.h"
#include "UI/ViewModel/MVVM_PlayerAbilityStates.h"
#include "UI/ViewModel/MVVM_PlayerState.h"
#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "UI/Widget/AuraMenuWidget.h"
#include "UI/Widget/FormWheelWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"


void AElectricCastleHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AElectricCastleHUD::Initialize()
{
	InitializePlayerViewModels();
	InitializeOverlayWidget();
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
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		InAbilitySystemComponent
	))
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

TArray<UMVVM_PlayerState*> AElectricCastleHUD::GetPlayerStateViewModels() const
{
	return PlayerStateViewModels;
}

TArray<UMVVM_PlayerAbilityStates*> AElectricCastleHUD::GetPlayerAbilityStatesViewModels() const
{
	return PlayerAbilityStatesViewModels;
}

TArray<UMVVM_PlayerForms*> AElectricCastleHUD::GetPlayerFormsViewModels() const
{
	return PlayerFormsViewModels;
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

UMVVM_PlayerState* AElectricCastleHUD::CreatePlayerStateViewModel(
	int32 PlayerIdx,
	AElectricCastlePlayerState* PlayerState
)
{
	UMVVM_PlayerState* PlayerStateViewModel = NewObject<UMVVM_PlayerState>(this, PlayerStateViewModelClass);
	PlayerStateViewModel->SetPlayerIndex(PlayerIdx);
	PlayerStateViewModel->InitializeDependencies(PlayerState);
	return PlayerStateViewModel;
}

UMVVM_PlayerAbilityStates* AElectricCastleHUD::CreatePlayerAbilityStatesViewModel(
	int32 PlayerIdx,
	AElectricCastlePlayerState* PlayerState
)
{
	UMVVM_PlayerAbilityStates* PlayerAbilityStatesViewModel = NewObject<UMVVM_PlayerAbilityStates>(
		this,
		PlayerAbilityStatesViewModelClass
	);
	PlayerAbilityStatesViewModel->SetPlayerIndex(PlayerIdx);
	PlayerAbilityStatesViewModel->InitializeDependencies(PlayerState);
	return PlayerAbilityStatesViewModel;
}


UMVVM_PlayerForms* AElectricCastleHUD::CreatePlayerFormsViewModel(
	const int32 PlayerIndex,
	AElectricCastlePlayerState* PlayerState
)
{
	if (PlayerFormsViewModelClass == nullptr)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerFormsViewModelClass is null"), *GetName());
		return nullptr;
	}
	UMVVM_PlayerForms* PlayerFormsViewModel = NewObject<UMVVM_PlayerForms>(
		this,
		PlayerFormsViewModelClass
	);
	PlayerFormsViewModel->SetPlayerIndex(PlayerIndex);
	PlayerFormsViewModel->InitializeDependencies(PlayerState);
	return PlayerFormsViewModel;
}

void AElectricCastleHUD::InitializePlayerViewModels()
{
	if (!PlayerStateViewModelClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerStateViewModelClass is null"), *GetName());
		return;
	}
	if (const AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>())
	{
		// this should maybe be done on player state begin play?
		for (int32 PlayerIdx = 0; PlayerIdx < GameState->PlayerArray.Num(); PlayerIdx++)
		{
			if (AElectricCastlePlayerState* PlayerState = Cast<AElectricCastlePlayerState>(
				GameState->PlayerArray[PlayerIdx]
			))
			{
				PlayerStateViewModels.Add(CreatePlayerStateViewModel(PlayerIdx, PlayerState));
				PlayerAbilityStatesViewModels.Add(CreatePlayerAbilityStatesViewModel(PlayerIdx, PlayerState));
				PlayerFormsViewModels.Add(CreatePlayerFormsViewModel(PlayerIdx, PlayerState));
			}
		}
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] GameState is null"), *GetName());
	}
}

void AElectricCastleHUD::InitializeInventoryViewModel()
{
	InventoryViewModel = NewObject<UMVVM_Inventory>(this, InventoryViewModelClass);
	InventoryViewModel->InitializeInventoryItems();
	InventoryViewModel->InitializeDependencies();
}

void AElectricCastleHUD::InitializeOverlayWidget()
{
	if (!OverlayWidgetClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] OverlayWidgetClass is null"), *GetName());
		return;
	}
	OverlayWidget = CreateWidget<UOverlayWidget>(GetWorld(), OverlayWidgetClass, FName("OverlayWidget"));
	OverlayWidget->BindViewModels(
		GetPlayerStateViewModels(),
		GetPlayerAbilityStatesViewModels(),
		GetPlayerFormsViewModels()
	);
	OverlayWidget->AddToViewport();
}

void AElectricCastleHUD::InitializeAttributeWidgetController(const FWidgetControllerParams& Params)
{
	if (!AttributeMenuWidgetController)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(
			this,
			AttributeMenuWidgetControllerClass
		);
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
