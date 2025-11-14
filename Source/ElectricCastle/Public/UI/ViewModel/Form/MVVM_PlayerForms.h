// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/Form/FormConfigTypes.h"
#include "MVVM_PlayerForms.generated.h"

struct FPlayerFormConfigRow;
class UMVVM_PlayerForm;
class AElectricCastlePlayerState;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UMVVM_PlayerForms : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	int32 GetPlayerIndex() const { return PlayerIndex; };
	void SetPlayerIndex(const int32 InPlayerIndex) { PlayerIndex = InPlayerIndex; };
	void InitializeDependencies(AElectricCastlePlayerState* PlayerState);
	FName CreateWidgetName(const FString& InWidgetName) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<UMVVM_PlayerForm> PlayerFormViewModelClass;
	UPROPERTY()
	TMap<EPlayerForm, TObjectPtr<UMVVM_PlayerForm>> PlayerFormViewModels;

private:
	int32 PlayerIndex = 0;

	void CreatePlayerFormViewModels(AElectricCastlePlayerState* PlayerState);
	UMVVM_PlayerForm* CreatePlayerFormViewModel(
		AElectricCastlePlayerState* PlayerState,
		const FPlayerFormConfigRow& FormConfigRow
	);
};
