// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "AuraOverlayWidget.generated.h"


enum class EAuraMenuTab : uint8;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOverlayOpenMenuSignature, const EAuraMenuTab&, OpenTab);

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAuraOverlayWidget : public UAuraUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Show();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Hide();

	UFUNCTION(BlueprintCallable)
	void RequestOpenMenu(const EAuraMenuTab OpenTab);
	FOverlayOpenMenuSignature OnOpenMenuDelegate;
};
