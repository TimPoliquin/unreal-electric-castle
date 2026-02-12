// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonAnalogCursor.h"
#include "ElectricCastleCommonUIActionRouter.generated.h"

class FElectricCastleCommonAnalogCursor : public FCommonAnalogCursor
{
public:
	explicit FElectricCastleCommonAnalogCursor(const UCommonUIActionRouterBase& InActionRouter) : FCommonAnalogCursor(InActionRouter)
	{
	}

	virtual bool ShouldVirtualAcceptSimulateMouseButton(const FKeyEvent& InKeyEvent, EInputEvent InputEvent) const override;
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleCommonUIActionRouter : public UCommonUIActionRouterBase
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<FCommonAnalogCursor> MakeAnalogCursor() const override;
};
