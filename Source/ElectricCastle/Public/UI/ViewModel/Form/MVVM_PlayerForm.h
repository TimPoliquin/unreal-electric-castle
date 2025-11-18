// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "Player/Form/PlayerFormConfig.h"
#include "Player/Form/PlayerFormDelegates.h"
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

	FGameplayTag GetFormTag() const;
	void SetFormTag(const FGameplayTag& InFormTag);
	FString GetFormName() const;
	void SetFormName(const FString& InFormName);
	UTexture2D* GetFormIcon() const;
	void SetFormIcon(UTexture2D* InFormIcon);
	UFUNCTION(BlueprintCallable, BlueprintPure, FieldNotify)
	FSlateBrush GetFormIconBrush() const;
	bool GetIsSelected() const;
	void SetIsSelected(const bool InIsSelected);
	bool GetIsAvailable() const;
	void SetIsAvailable(const bool InIsAvailable);

protected:
	UPROPERTY(BlueprintReadOnly)
	EPlayerForm FormId = EPlayerForm::None;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FGameplayTag FormTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString FormName;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	TObjectPtr<UTexture2D> FormIcon;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	bool IsSelected = false;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	bool IsAvailable = false;

private:
	UFUNCTION()
	void OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload);
	UFUNCTION()
	void OnPlayerFormChanged(const FPlayerFormChangeEventPayload& Payload);
};
