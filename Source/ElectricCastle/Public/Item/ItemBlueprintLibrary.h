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
	UFUNCTION(BlueprintCallable, Category="AuraItemBlueprintLibrary|Items")
	static FItemDefinition GetItemDefinitionByItemType(
		const UObject* WorldContextObject,
		const FGameplayTag& ItemType
	);
	UFUNCTION(BlueprintCallable, Category="AuraItemBlueprintLibrary|Items")
	static FString GetItemNameByItemType(const UObject* WorldContextObject, const FGameplayTag& ItemType);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraItemBlueprintLibrary|Messages")
	static FString Substitute(const FString& Message, const FMessageSubstitutions& MessageSubstitutions);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraItemBlueprintLibrary|Messages")
	static UTexture2D* SubstituteMessageIcon(UTexture2D* MessageIcon, const FMessageSubstitutions& MessageSubstitutions);

	static UFishCatch* ToFishCatch(const UObject* WorldContextObject, const FGameplayTag& FishType);
};
