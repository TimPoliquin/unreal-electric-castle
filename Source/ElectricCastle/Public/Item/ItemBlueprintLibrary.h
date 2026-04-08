// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ItemBlueprintLibrary.generated.h"

struct FItemDefinition;
class UFishCatch;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UItemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ItemBlueprintLibrary|Items")
	static FItemDefinition GetItemDefinitionByItemType(
		const UObject* WorldContextObject,
		const FGameplayTag& ItemType
	);
	UFUNCTION(BlueprintCallable, Category="ItemBlueprintLibrary|Items")
	static FString GetItemNameByItemType(const UObject* WorldContextObject, const FGameplayTag& ItemType);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ItemBlueprintLibrary|Messages")
	static FString Substitute(const FString& Message, const FMessageSubstitutions& MessageSubstitutions);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ItemBlueprintLibrary|Messages")
	static UTexture2D* SubstituteMessageIcon(UTexture2D* MessageIcon, const FMessageSubstitutions& MessageSubstitutions);
};
