// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Player/SelectionWheel/SelectionWheelSubscriberInterface.h"
#include "Dialog/DialogTypes.h"
#include "DialogWheelWidget.generated.h"

class UDialogChoiceWidget;
class URadialLayout;
class URadialLayoutCursor;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FDialogChoiceConfirmedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SelectionTag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogChoiceConfirmedSignature, const FDialogChoiceConfirmedPayload&, Payload);

/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UDialogWheelWidget : public UUserWidget, public ISelectionWheelSubscriberInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Show(const bool bAnimate = true);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hide(const bool bAnimate = true);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddDialogChoice(const FDialogChoice& InDialogChoice);

	UPROPERTY(BlueprintAssignable)
	FDialogChoiceConfirmedSignature OnDialogChoiceConfirmed;

	/** Start ISelectionWheelSubscriberInterface **/
	virtual void OnSelectionWheelAngleChange_Implementation(float Value) override;
	virtual void OnSelectionWheelConfirm_Implementation() override;
	/** End ISelectionWheelSubscriberInterface **/
protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UDialogChoiceWidget* GetDialogChoiceWidget(const FGameplayTag& DialogChoiceTag) const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	URadialLayout* GetRadialLayout() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	URadialLayoutCursor* GetCursorWidget() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ConfirmSelection();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AllDialogChoiceTags;

private:
	void UnregisterSelectionWheelListener() const;
};
