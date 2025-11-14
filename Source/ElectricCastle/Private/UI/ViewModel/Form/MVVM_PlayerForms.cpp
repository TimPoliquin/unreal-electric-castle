// Copyright Alien Shores


#include "UI/ViewModel/Form/MVVM_PlayerForms.h"
#include "Engine/Engine.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Form/PlayerFormConfig.h"


#include "UI/ViewModel/Form/MVVM_PlayerForm.h"

void UMVVM_PlayerForms::InitializeDependencies(AElectricCastlePlayerState* PlayerState)
{
	CreatePlayerFormViewModels(PlayerState);
}

FName UMVVM_PlayerForms::CreateWidgetName(const FString& InWidgetName) const
{
	return FName(*FString::Printf(TEXT("%s_%s"), *InWidgetName, *GetName()));
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
