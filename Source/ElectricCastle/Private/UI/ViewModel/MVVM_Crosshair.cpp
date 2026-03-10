// Copyright Alien Shores


#include "UI/ViewModel/MVVM_Crosshair.h"

#include "Actor/Highlight/HighlightActorInterface.h"
#include "Actor/Highlight/HighlightComponent.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Aim/AimActorInterface.h"
#include "Player/Aim/AimController.h"

void UMVVM_Crosshair::InitializeDependencies_Implementation(const AElectricCastlePlayerState* PlayerState)
{
	if (!IsValid(PlayerState))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] PlayerState is null"), *GetName());
		return;
	}
	if (UAimController* AimController = IAimActorInterface::GetAimController(PlayerState))
	{
		AimController->OnShowCrosshair.AddUniqueDynamic(this, &UMVVM_Crosshair::HandleCanAim);
		AimController->OnHideCrosshair.AddUniqueDynamic(this, &UMVVM_Crosshair::HandleCannotAim);
		AimController->OnAimStart.AddUniqueDynamic(this, &UMVVM_Crosshair::HandleAimStart);
		AimController->OnAimEnd.AddUniqueDynamic(this, &UMVVM_Crosshair::HandleAimEnd);
		AimController->OnTargetChanged.AddUniqueDynamic(this, &UMVVM_Crosshair::HandleTargetChange);
		SetCanAim(AimController->GetCanAim() && !AimController->GetHideCrosshair());
		SetIsAiming(AimController->IsAiming());
	}
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCrosshairStyle);
}

ECrosshairState UMVVM_Crosshair::GetCrosshairState() const
{
	if (!CanAim)
	{
		return ECrosshairState::Hidden;
	}
	if (IsAiming || IsFiring)
	{
		return ECrosshairState::Active;
	}
	return ECrosshairState::Subtle;
}

FCrosshairStyle UMVVM_Crosshair::GetCrosshairStyle() const
{
	return CrosshairStyle;
}

void UMVVM_Crosshair::HandleTargetChange_Implementation(const FTargetChangedPayload& Payload)
{
	if (const UHighlightComponent* HighlightComponent = IHighlightActorInterface::GetHighlightComponent(Payload.Target))
	{
		CrosshairStyle.Style = ECrosshairStyle::Targeting;
		CrosshairStyle.Color = HighlightComponent->GetHighlightColor();
	}
	else
	{
		CrosshairStyle.Style = ECrosshairStyle::Default;
		CrosshairStyle.Color = DefaultCrosshairColor;
	}
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCrosshairStyle);
}

void UMVVM_Crosshair::HandleAimEnd_Implementation()
{
	SetIsAiming(false);
}

void UMVVM_Crosshair::HandleAimStart_Implementation()
{
	SetIsAiming(true);
}

void UMVVM_Crosshair::HandleCannotAim_Implementation()
{
	SetCanAim(false);
}

void UMVVM_Crosshair::HandleCanAim_Implementation()
{
	SetCanAim(true);
}

bool FCrosshairStyle::operator==(const FCrosshairStyle& CrosshairStyle) const
{
	return Style == CrosshairStyle.Style && Color == CrosshairStyle.Color;
}

bool UMVVM_Crosshair::GetCanAim() const
{
	return CanAim;
}

void UMVVM_Crosshair::SetCanAim(const bool InCanAim)
{
	UE_MVVM_SET_PROPERTY_VALUE(CanAim, InCanAim);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCrosshairState);
}

bool UMVVM_Crosshair::GetIsAiming() const
{
	return IsAiming;
}

void UMVVM_Crosshair::SetIsAiming(const bool InIsAiming)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsAiming, InIsAiming);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCrosshairState);
}

bool UMVVM_Crosshair::GetIsFiring() const
{
	return IsFiring;
}

void UMVVM_Crosshair::SetIsFiring(const bool InIsFiring)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsFiring, InIsFiring);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCrosshairState);
}
