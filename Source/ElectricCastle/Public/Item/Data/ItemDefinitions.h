// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Item/ItemTypes.h"
#include "ItemDefinitions.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UItemDefinitions : public UDataAsset
{
	GENERATED_BODY()

public:
	FItemDefinition FindItemByItemType(const FGameplayTag& ItemType) const;
	void AddToMap(TMap<FGameplayTag, FItemDefinition>& ItemDefinitionMap) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TArray<FItemDefinition> ItemInfo;
};
