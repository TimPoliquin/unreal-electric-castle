#include "Player/InputEvents.h"

AElectricCastlePlayerController* FOnPlayerFormWheelVisibilityChangePayload::GetPlayerController() const
{
	if (PlayerController.IsValid())
	{
		return PlayerController.Get();
	}
	return nullptr;
}

AElectricCastlePlayerState* FOnPlayerFormWheelVisibilityChangePayload::GetPlayerState() const
{
	if (PlayerState.IsValid())
	{
		return PlayerState.Get();
	}
	return nullptr;
}

AElectricCastlePlayerCharacter* FOnPlayerFormWheelVisibilityChangePayload::GetPlayerCharacter() const
{
	if (PlayerCharacter.IsValid())
	{
		return PlayerCharacter.Get();
	}
	return nullptr;
}

bool FOnPlayerFormWheelVisibilityChangePayload::IsValid() const
{
	return PlayerController.IsValid() && PlayerState.IsValid() && PlayerCharacter.IsValid();
}

bool FOnPlayerFormWheelVisibilityChangePayload::IsVisible() const
{
	return bIsVisible;
}

bool FOnPlayerFormWheelVisibilityChangePayload::IsHidden() const
{
	return !bIsVisible;
}
