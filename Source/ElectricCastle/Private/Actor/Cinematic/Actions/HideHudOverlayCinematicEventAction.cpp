// Copyright Alien Shores


#include "Actor/Cinematic/Actions/HideHudOverlayCinematicEventAction.h"

#include "Cinematic/Context/CinematicContextHandle.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/HUD/ElectricCastleHUD.h"
#include "UI/HUD/OverlayWidget.h"

UHideHudOverlayCinematicEventAction::UHideHudOverlayCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_HideGameOverlay);
}

void UHideHudOverlayCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	if (const AElectricCastleHUD* HUD = Cast<AElectricCastleHUD>(GetOwner()))
	{
		if (UOverlayWidget* Overlay = HUD->GetOverlayWidget())
		{
			if (Overlay->IsVisible())
			{
				Overlay->Hide();
				ContextHandle->AddRestoreLambda(Overlay, true, [Overlay](UOverlayWidget* InOverlay, bool bInValue)
				{
					InOverlay->Show();
				});
			}
		}
	}
}
