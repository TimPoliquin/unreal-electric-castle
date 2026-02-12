// Copyright Alien Shores


#include "Input/ElectricCastleCommonUIActionRouter.h"

bool FElectricCastleCommonAnalogCursor::ShouldVirtualAcceptSimulateMouseButton(const FKeyEvent& InKeyEvent, EInputEvent InputEvent) const
{
	const ECommonInputMode InputMode = ActionRouter.GetActiveInputMode();
	return InputMode != ECommonInputMode::Game;
}

TSharedRef<FCommonAnalogCursor> UElectricCastleCommonUIActionRouter::MakeAnalogCursor() const
{
	return FCommonAnalogCursor::CreateAnalogCursor<FElectricCastleCommonAnalogCursor>(*this);
}
