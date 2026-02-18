// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Input/RadialInputListenerInterface.h"
#include "RadialLayoutCursor.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API URadialLayoutCursor : public UUserWidget, public IRadialInputListenerInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Start IRadialInputListenerInterface **/
	virtual void OnRadialInputAngleChange_Implementation(float Value) override;
	/** End IRadialInputListenerInterface **/
};
