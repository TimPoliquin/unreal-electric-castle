// Copyright Alien Shores

#include "Actor/MagicHand/Handler/TetherUpdateHandler.h"

void UTetherUpdateHandler::Initialize_Implementation()
{
	// nothing to do natively
}

FVector UTetherUpdateHandler::QueryAllowedMovement_Implementation(
	const FTetherHandlerContext& Context, const FVector& DesiredInputVector
)
{
	return DesiredInputVector;
}

FTetherHandlerResult UTetherUpdateHandler::UpdateTetherPhysics_Implementation(
	const FTetherHandlerContext& Context
)
{
	return FTetherHandlerResult{};
}
