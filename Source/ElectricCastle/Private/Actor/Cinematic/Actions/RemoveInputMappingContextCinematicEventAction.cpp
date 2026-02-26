// Copyright Alien Shores


#include "Actor/Cinematic/Actions/RemoveInputMappingContextCinematicEventAction.h"

#include "EnhancedInputSubsystems.h"
#include "Cinematic/Context/CinematicContextHandle.h"
#include "Tags/ElectricCastleGameplayTags.h"

URemoveInputMappingContextCinematicEventAction::URemoveInputMappingContextCinematicEventAction()
{
	Triggers.AddTag(FElectricCastleGameplayTags::Get().Cinematic_Reaction_Trigger_DisableStandardInput);
}

void URemoveInputMappingContextCinematicEventAction::Execute_Implementation(const UCinematicContextHandle* ContextHandle) const
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetOwnerController());
	const ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		if (Subsystem->HasMappingContext(InputMappingContext))
		{
			ContextHandle->AddRestoreLambda(Subsystem, InputMappingContext, [](UEnhancedInputLocalPlayerSubsystem* InSubsystem, const UInputMappingContext* InMappingContext)
			{
				InSubsystem->AddMappingContext(InMappingContext, 0);
			});
			Subsystem->RemoveMappingContext(InputMappingContext);
		}
	}
}
