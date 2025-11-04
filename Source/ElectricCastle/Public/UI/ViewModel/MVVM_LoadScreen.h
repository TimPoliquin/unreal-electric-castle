// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelectedSignature);

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;
	UPROPERTY(BlueprintAssignable)
	FSlotSelectedSignature SlotSelectedDelegate;

	void InitializeLoadSlots();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotByIndex(const int32 Index) const
	{
		checkf(Index >= 0, TEXT("LoadSlot index must be greater than 0"));
		checkf(Index < LoadSlots.Num(), TEXT("LoadSlot index must be less than %d"), LoadSlots.Num());
		return LoadSlots[Index];
	}

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(const int32 SlotIndex, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(const int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(const int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void ClearSelectedSlot();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	void LoadData();
	ULocalPlayer* GetLocalPlayer() const;
	void SetLocalPlayer(class ULocalPlayer* InLocalPlayer);

private:
	UPROPERTY()
	TArray<UMVVM_LoadSlot*> LoadSlots;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> SelectedSlot;
	UPROPERTY()
	TObjectPtr<ULocalPlayer> LocalPlayer;
};
