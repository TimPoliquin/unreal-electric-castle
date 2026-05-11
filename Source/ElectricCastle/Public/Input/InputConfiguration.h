// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InputConfiguration.generated.h"

struct FInputConfigurationRow;
class UInputAction;

USTRUCT(BlueprintType)
struct FInputConfigurationRow
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;
	UPROPERTY(EditDefaultsOnly, meta=(Categories="Input"))
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly)
	TArray<ETriggerEvent> TriggerEvents = {ETriggerEvent::Started, ETriggerEvent::Triggered, ETriggerEvent::Completed};
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UInputConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<FInputConfigurationRow> AbilityInputActions;
};
