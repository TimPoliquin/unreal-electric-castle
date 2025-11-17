// Copyright Alien Shores


#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "Engine/Engine.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Player/ElectricCastlePlayerController.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "Player/Form/PlayerFormConfig.h"


#include "UI/ViewModel/Form/MVVM_PlayerForm.h"

void UMVVM_PlayerForms::InitializeDependencies(AElectricCastlePlayerState* PlayerState)
{
	CreatePlayerFormViewModels(PlayerState);
	if (AElectricCastlePlayerController* PlayerController = Cast<AElectricCastlePlayerController>(PlayerState->GetPlayerController()))
	{
		PlayerController->OnFormWheelVisibilityChange.AddUniqueDynamic(this, &UMVVM_PlayerForms::OnFormWheelVisibilityChange);
	}
	if (UPlayerFormChangeComponent* FormChangeComponent = IFormChangeActorInterface::GetFormChangeComponent(PlayerState))
	{
		FormChangeComponent->OnAvailableFormsChanged.AddUniqueDynamic(this, &UMVVM_PlayerForms::OnAvailableFormsChanged);
	}
}

FName UMVVM_PlayerForms::CreateWidgetName(const FString& InWidgetName) const
{
	return FName(*FString::Printf(TEXT("%s_%s"), *InWidgetName, *GetName()));
}

UMVVM_PlayerForm* UMVVM_PlayerForms::GetPlayerFormViewModelById(const EPlayerForm Form) const
{
	return PlayerFormViewModels[Form];
}

UMVVM_PlayerForm* UMVVM_PlayerForms::GetPlayerFormViewModelByTag(const FGameplayTag& FormTag) const
{
	for (const auto& Pair : PlayerFormViewModels)
	{
		if (Pair.Value->GetFormTag() == FormTag)
		{
			return Pair.Value;
		}
	}
	return nullptr;
}

TArray<UMVVM_PlayerForm*> UMVVM_PlayerForms::GetAvailablePlayerFormViewModels() const
{
	TArray<UMVVM_PlayerForm*> AvailablePlayerFormViewModels;
	for (const auto& Pair : PlayerFormViewModels)
	{
		if (Pair.Value->GetIsAvailable())
		{
			AvailablePlayerFormViewModels.Add(Pair.Value);
		}
	}
	return AvailablePlayerFormViewModels;
}

TArray<UMVVM_PlayerForm*> UMVVM_PlayerForms::GetPlayerFormViewModels() const
{
	TArray<UMVVM_PlayerForm*> AllViewModels;
	for (const auto& Pair : PlayerFormViewModels)
	{
		AllViewModels.Add(Pair.Value);
	}
	return AllViewModels;
}

void UMVVM_PlayerForms::CreatePlayerFormViewModels(AElectricCastlePlayerState* PlayerState)
{
	if (PlayerFormViewModels.IsEmpty())
	{
		if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(PlayerState))
		{
			UPlayerFormConfig* FormConfig = GameDataSubsystem->GetPlayerFormConfig();
			for (const EPlayerForm& Form : TEnumRange<EPlayerForm>())
			{
				PlayerFormViewModels.Add(
					Form,
					CreatePlayerFormViewModel(
						PlayerState,
						FormConfig->GetPlayerFormConfigRowByFormId(Form)
					)
				);
			}
		}
	}
}

UMVVM_PlayerForm* UMVVM_PlayerForms::CreatePlayerFormViewModel(
	AElectricCastlePlayerState* PlayerState,
	const FPlayerFormConfigRow& FormConfigRow
)
{
	const FString ViewModelName = FString::Printf(
		TEXT("FormViewModel_%s"),
		*UEnum::GetValueAsString(FormConfigRow.FormId)
	);
	UMVVM_PlayerForm* ViewModel = NewObject<UMVVM_PlayerForm>(this, PlayerFormViewModelClass, FName(ViewModelName));
	ViewModel->InitializeDependencies(PlayerState, FormConfigRow);
	return ViewModel;
}

void UMVVM_PlayerForms::OnFormWheelVisibilityChange(const FOnPlayerFormWheelVisibilityChangePayload& Payload)
{
	OnVisibilityChange.Broadcast(Payload.bIsVisible);
}

void UMVVM_PlayerForms::OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload)
{
	OnAvailableFormsChangedDelegate.Broadcast(Payload);
}
