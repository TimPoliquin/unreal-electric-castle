// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Dialog/DialogTypes.h"
#include "UI/Widget/HoverableWidget.h"
#include "DialogChoiceWidget.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UDialogChoiceWidget : public UUserWidget, public IHoverableWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetDialogChoiceTag() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetDialogChoice(const FDialogChoice& DialogChoice);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetDialogChoiceRaw(const FGameplayTag& InDialogChoiceTag, const FText& InText, UTexture2D* InIcon);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsConfigured() const;

	/** Start IHoverableWidget **/
	virtual void Hover_Implementation() override;
	virtual void Unhover_Implementation() override;
	/** End IHoverableWidget **/

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetChoiceText(const FText& InChoiceText);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetChoiceIcon(UTexture2D* InChoiceIcon);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(Categories="InputTag.Dialog"))
	FGameplayTag DialogChoiceTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FText Text;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	TObjectPtr<UTexture2D> Icon;
};
