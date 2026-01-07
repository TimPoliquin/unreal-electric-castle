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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FGameplayTag StatusEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	float Duration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FName MaterialProgressParameter = FName("Alpha");
	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* CreateDynamicMaterial(UImage* Image);
private:
	float Runtime = 0.f;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ProgressMaterialDynamic;
};
