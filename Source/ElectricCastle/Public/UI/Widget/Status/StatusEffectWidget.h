// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UStatusEffectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FGameplayTag GetStatusEffectTag() const;
	void SetStatusEffectTag(const FGameplayTag& InStatusEffectTag);
	void SetDuration(const float InDuration);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIconFromTexture2D(UTexture2D* Icon);
	UFUNCTION(BlueprintNativeEvent)
	void SetBackgroundColor(FLinearColor InBackgroundColor);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FGameplayTag StatusEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	float Duration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FName MaterialProgressParameter = FName("Alpha");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FName MaterialColorParameter = FName("Color");
	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* CreateDynamicMaterial(UImage* Image);
	UFUNCTION(BlueprintImplementableEvent)
	void SetIcon(FSlateBrush IconBrush);

private:
	float Runtime = 0.f;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ProgressMaterialDynamic;
	FLinearColor BackgroundColor = FLinearColor::White;
};
