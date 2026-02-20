// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/SelectionWheel/SelectionWheelSubscriberInterface.h"
#include "RadialLayoutCursor.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API URadialLayoutCursor : public UUserWidget, public ISelectionWheelSubscriberInterface
{
	GENERATED_BODY()

public:
	/** Start IRadialInputListenerInterface **/
	virtual void OnSelectionWheelAngleChange_Implementation(float Value) override;
	virtual void OnSelectionWheelConfirm_Implementation() override;
	/** End IRadialInputListenerInterface **/
};
