// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_PlayerAbilityStates.generated.h"

class UMVVM_AbilityState;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UMVVM_PlayerAbilityStates : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeDependencies(APlayerState* PlayerState);
	void SetPlayerIndex(const int32 InPlayerIndex) { PlayerIndex = InPlayerIndex; };

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<UMVVM_AbilityState> AbilityStateViewModelClass;
	UPROPERTY(BlueprintReadOnly, Category = "Properties")
	int32 PlayerIndex = 0;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_AbilityState> Ability_01;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_AbilityState> Ability_02;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_AbilityState> Ability_03;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_AbilityState> Ability_04;
};
