// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FormConfig.generated.h"

class UPlayerFormChangeComponent;
/**
 * 
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, Blueprintable)
class ELECTRICCASTLE_API UFormConfig : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnFormActivated(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent);
	UFUNCTION(BlueprintNativeEvent)
	void OnFormDeactivated(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent);
};
