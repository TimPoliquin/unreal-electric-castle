// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionWidgetInterface.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE()
class UInteractionWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IInteractionWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hide();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInteractionKey(const FString& Key);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetActionText(const FString& InActionText);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetStartHidden(const bool bInStartHidden);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetAutoDestroyOnHide(const bool bInAutoDestroyOnHide);

	static void Show(UUserWidget* Widget);
	static void Hide(UUserWidget* Widget);
};
