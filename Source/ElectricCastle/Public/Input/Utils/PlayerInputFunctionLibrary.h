// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerInputFunctionLibrary.generated.h"

class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPlayerInputFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="PlayerInputFunctionLibrary")
	static UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category="PlayerInputFunctionLibrary")
	static void AddInputMappingContext(UObject* WorldContextObject, const UInputMappingContext* InputMappingContext, const int32 Priority = 0);
	UFUNCTION(BlueprintCallable, Category="PlayerInputFunctionLibrary")
	static void RemoveInputMappingContext(UObject* WorldContextObject, const UInputMappingContext* InputMappingContext);
};
