// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/Form/PlayerFormConfig.h"
#include "MVVM_PlayerForm.generated.h"

class AElectricCastlePlayerState;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UMVVM_PlayerForm : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeDependencies(AElectricCastlePlayerState* PlayerState, const FPlayerFormConfigRow& FormConfigRow);
};
