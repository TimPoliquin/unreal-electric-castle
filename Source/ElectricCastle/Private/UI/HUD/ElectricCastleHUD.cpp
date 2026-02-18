// Copyright Alien Shores


#include "UI/HUD/ElectricCastleHUD.h"

#include "Blueprint/UserWidget.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/ElectricCastleGameState.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "Player/ElectricCastlePlayerState.h"
#include "UI/HUD/OverlayWidget.h"
#include "UI/ViewModel/MVVM_Inventory.h"
#include "UI/ViewModel/MVVM_PlayerAbilityStates.h"
#include "UI/ViewModel/MVVM_PlayerState.h"
#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "UI/Widget/AuraMenuWidget.h"
#include "UI/Widget/Loading/LoadingScreenWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"


void AElectricCastleHUD::BeginPlay()
{
	Super::BeginPlay();
	if (LoadingScreenWidgetClass)
	{
		LoadingScreenWidget = CreateWidget<ULoadingScreenWidget>(GetWorld(), LoadingScreenWidgetClass, FName("LoadingScreenWidget"));
	}
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetWorld()))
	{
		if (!GameDataSubsystem->IsGameDataLoaded())
		{
			if (LoadingScreenWidget)
			{
				LoadingScreenWidget->Show();
			}
			GameDataSubsystem->OnGameDataLoaded.AddUniqueDynamic(this, &AElectricCastleHUD::OnGameDataLoaded);
		}
		else
		{
			OnGameDataLoaded();
		}
	}
}

void AElectricCastleHUD::AddPlayerFormViewModel_Implementation(UMVVM_PlayerForms* PlayerFormsViewModel)
{
	PlayerFormsViewModels.Add(PlayerFormsViewModel);
	if (OverlayWidget)
	{
		OverlayWidget->BindPlayerFormsViewModel(PlayerFormsViewModel);
	}
}

void AElectricCastleHUD::AddPlayerStateViewModel_Implementation(UMVVM_PlayerState* PlayerStateViewModel)
{
	PlayerStateViewModels.Add(PlayerStateViewModel);
	if (OverlayWidget)
	{
		OverlayWidget->BindPlayerStateViewModel(PlayerStateViewModel);
	}
}

void AElectricCastleHUD::AddPlayerAbilityStateViewModel_Implementation(UMVVM_PlayerAbilityStates* PlayerAbilityStateViewModel)
{
	PlayerAbilityStatesViewModels.Add(PlayerAbilityStateViewModel);
	if (OverlayWidget)
	{
		OverlayWidget->BindPlayerAbilityStatesViewModel(PlayerAbilityStateViewModel);
	}
}

void AElectricCastleHUD::OnGameDataLoaded_Implementation()
{
	Initialize();
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->Hide();
	}
}

void AElectricCastleHUD::Initialize()
{
	InitializeOverlayWidget();
	InitializePlayerViewModels();
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

UOverlayWidget* AElectricCastleHUD::GetOverlayWidget() const
{
	return OverlayWidget;
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
	if (AElectricCastleGameState* GameState = GetWorld()->GetGameState<AElectricCastleGameState>())
	{
		// this should maybe be done on player state begin play?
		for (int32 PlayerIdx = 0; PlayerIdx < GameState->PlayerArray.Num(); PlayerIdx++)
		{
			if (AElectricCastlePlayerState* PlayerState = Cast<AElectricCastlePlayerState>(
				GameState->PlayerArray[PlayerIdx]
			))
			{
				InitializeViewModelsForPlayerState(PlayerState, PlayerIdx);
			}
		}
		GameState->OnPlayerStateAdded.AddUniqueDynamic(this, &AElectricCastleHUD::OnPlayerStateAdded);
		GameState->OnPlayerStateRemoved.AddUniqueDynamic(this, &AElectricCastleHUD::OnPlayerStateRemoved);
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] GameState is null"), *GetName());
	}
}

void AElectricCastleHUD::InitializeViewModelsForPlayerState(AElectricCastlePlayerState* PlayerState, const int32 PlayerIdx)
{
	AddPlayerStateViewModel(CreatePlayerStateViewModel(PlayerIdx, PlayerState));
	AddPlayerAbilityStateViewModel(CreatePlayerAbilityStatesViewModel(PlayerIdx, PlayerState));
	AddPlayerFormViewModel(CreatePlayerFormsViewModel(PlayerIdx, PlayerState));
}

void AElectricCastleHUD::OnPlayerStateAdded(const FGamePlayerStateAddedPayload& Payload)
{
	if (AElectricCastlePlayerState* PlayerState = Cast<AElectricCastlePlayerState>(Payload.PlayerState))
	{
		InitializeViewModelsForPlayerState(PlayerState, Payload.PlayerIndex);
		// TODO - invoke new function for each view model created
	}
}

void AElectricCastleHUD::OnPlayerStateRemoved(const FGamePlayerStateRemovedPayload& Payload)
{
	// TODO
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
	if (OverlayWidget)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] OverlayWidget already exists!"), *GetName());
		return;
	}
	OverlayWidget = CreateWidget<UOverlayWidget>(GetWorld(), OverlayWidgetClass, FName("OverlayWidget"));
	if (const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetWorld()))
	{
		if (GameDataSubsystem->IsGameDataLoaded())
		{
			OverlayWidget->AddToViewport();
		}
	}
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
