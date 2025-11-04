// Copyright Alien Shores


#include "Fishing/FishingBlueprintNode.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Fishing/FishInfo.h"
#include "Fishing/FishingComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/FishingActorInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Item/ItemBlueprintLibrary.h"
#include "Utils/RandUtils.h"


UFishingBlueprintNode* UFishingBlueprintNode::GoFishing(
	UObject* WorldContextObject,
	AActor* Player,
	const FGoFishingParams& Params
)
{
	UFishingBlueprintNode* BlueprintNode = NewObject<UFishingBlueprintNode>();
	BlueprintNode->PlayerActor = Player;
	BlueprintNode->GoFishingParams = Params;
	// Register with the game instance to avoid being garbage collected
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UFishingBlueprintNode::WaitForFishToBeLured()
{
	FTimerDelegate InterestTimerDelegate;
	InterestTimerDelegate.BindLambda(
		[this]()
		{
			OnFishingLuredDelegate.Broadcast(PlayerActor, CaughtFish);
		}
	);
	PlayerActor->GetWorld()->GetTimerManager().SetTimer(
		FishInterestToLureTimerHandle,
		InterestTimerDelegate,
		GoFishingParams.TimeToLure.Value(),
		false
	);
}

void UFishingBlueprintNode::LureAndWaitForABite(const FGameplayTag& FishType)
{
	ActiveFishType = FishType;
	SetFishState(EFishState::Lured);
	FTimerDelegate LureCallbackDelegate;
	LureCallbackDelegate.BindLambda(
		[this]()
		{
			BiteAndWaitForPlayerOrFlee();
		}
	);
	PlayerActor->GetWorld()->GetTimerManager().SetTimer(
		FishLureToBiteTimerHandle,
		LureCallbackDelegate,
		GoFishingParams.LureToBiteTime.Value(),
		false
	);
}

void UFishingBlueprintNode::BiteAndWaitForPlayerOrFlee()
{
	SetFishState(EFishState::Biting);
	FishingEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(PlayerActor, GoFishingParams.ReelEffectClass);
	OnFishingBiteDelegate.Broadcast(PlayerActor, CaughtFish);
	FTimerDelegate BiteToFleeCallbackDelegate;
	BiteToFleeCallbackDelegate.BindLambda(
		[this]()
		{
			Flee();
		}
	);
	PlayerActor->GetWorld()->GetTimerManager().SetTimer(
		FishBiteToFleeTimerHandle,
		BiteToFleeCallbackDelegate,
		GoFishingParams.BiteToFleeTime.Value(),
		false
	);
}

void UFishingBlueprintNode::Flee()
{
	ActiveFishType = FGameplayTag::EmptyTag;
	SetFishState(EFishState::Fled);
	UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(PlayerActor, FishingEffectHandle);
	OnFishingFishHasFledDelegate.Broadcast(PlayerActor, CaughtFish);
}

void UFishingBlueprintNode::Reel()
{
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishBiteToFleeTimerHandle);
	SetFishState(EFishState::Fighting);
	UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(PlayerActor, FishingEffectHandle);
	OnFishingFishReelingDelegate.Broadcast(PlayerActor, CaughtFish);
}

void UFishingBlueprintNode::Catch()
{
	CaughtFish = UItemBlueprintLibrary::ToFishCatch(PlayerActor, ActiveFishType);
	SetFishState(EFishState::Caught);
	OnFishingFishCaughtDelegate.Broadcast(PlayerActor, CaughtFish);
}


void UFishingBlueprintNode::Activate()
{
	if (UFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(PlayerActor))
	{
		FishingComponent->GetOnFishingStateChangedDelegate().AddDynamic(
			this,
			&UFishingBlueprintNode::OnFishingStateChanged
		);
		if (!FishingComponent->IsFishing())
		{
			MoveCameraToPosition();
			MovePlayerToPosition();
		}
		else
		{
			OnPlayerInPositionDelegate.Broadcast(PlayerActor, CaughtFish);
		}
	}
}

void UFishingBlueprintNode::Cleanup()
{
	ActiveFishType = FGameplayTag::EmptyTag;
	UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(PlayerActor, FishingEffectHandle);
	OnCameraInPositionDelegate.Clear();
	OnFishingCancelledDelegate.Clear();
	OnPlayerInPositionDelegate.Clear();
	OnFishingRodEquippedDelegate.Clear();
	OnFishingRodCastDelegate.Clear();
	OnFishingLuredDelegate.Clear();
	OnFishingBiteDelegate.Clear();
	OnFishingFishHasFledDelegate.Clear();
	OnFishingFishReelingDelegate.Clear();
	OnFishingFishCaughtDelegate.Clear();
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(PlayerMoveToTargetTimerHandle);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishInterestToLureTimerHandle);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishLureToBiteTimerHandle);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishBiteToFleeTimerHandle);
}

void UFishingBlueprintNode::PrepareForContinue()
{
	SetFishState(EFishState::None);
	ActiveFishType = FGameplayTag::EmptyTag;
	Cleanup();
	if (UFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(PlayerActor))
	{
		FishingComponent->OnFishingStateChangedDelegate.RemoveDynamic(
			this,
			&UFishingBlueprintNode::OnFishingStateChanged
		);
		FishingComponent->PrepareForContinue();
	}
}

void UFishingBlueprintNode::End()
{
	OnFishingCancelledDelegate.Broadcast(PlayerActor, CaughtFish);
	Cleanup();
	if (UFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(PlayerActor))
	{
		FishingComponent->OnFishingStateChangedDelegate.RemoveDynamic(
			this,
			&UFishingBlueprintNode::OnFishingStateChanged
		);
	}
	SetReadyToDestroy();
}

void UFishingBlueprintNode::MoveCameraToPosition()
{
	if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(PlayerActor))
	{
		OnCameraMoveFinishedDelegate.BindUObject(this, &UFishingBlueprintNode::OnCameraInPosition);
		PlayerInterface->MoveCameraToPointWithCallback(
			GoFishingParams.CameraTarget->GetActorLocation(),
			GoFishingParams.CameraTarget->GetActorForwardVector(),
			GoFishingParams.CameraMovementCurve,
			OnCameraMoveFinishedDelegate
		);
	}
}

void UFishingBlueprintNode::MovePlayerToPosition()
{
	if (APawn* Pawn = Cast<APawn>(PlayerActor))
	{
		// TODO - do something more robust here via the IFishingInterface
		PlayerMoveToTargetTimerDelegate.BindLambda(
			[this]()
			{
				OnPlayerInPositionDelegate.Broadcast(PlayerActor, CaughtFish);
			}
		);
		UAIBlueprintHelperLibrary::SimpleMoveToActor(Pawn->GetController(), GoFishingParams.PlayerTarget);
		Pawn->GetWorld()->GetTimerManager().SetTimer(
			PlayerMoveToTargetTimerHandle,
			PlayerMoveToTargetTimerDelegate,
			1.f,
			false
		);
		ICombatInterface::UpdateFacingTarget(Pawn, GoFishingParams.FishingTarget->GetActorLocation());
	}
}


void UFishingBlueprintNode::OnCameraInPosition()
{
	OnCameraInPositionDelegate.Broadcast(PlayerActor, CaughtFish);
}

void UFishingBlueprintNode::OnFishingRodEquipped()
{
	OnFishingRodEquippedDelegate.Broadcast(PlayerActor, CaughtFish);
}

void UFishingBlueprintNode::OnFishingRodCast()
{
	OnFishingRodCastDelegate.Broadcast(PlayerActor, CaughtFish);
	WaitForFishToBeLured();
}

void UFishingBlueprintNode::OnFishingStateChanged(EFishingState FishingState)
{
	UE_LOG(LogElectricCastle, Warning, TEXT("Fishing state has changed: [%s]"), *UEnum::GetDisplayValueAsText(FishingState).ToString());
	switch (FishingState)
	{
	case EFishingState::Equipped:
		OnFishingRodEquipped();
		break;
	case EFishingState::Cast:
		OnFishingRodCast();
		break;
	case EFishingState::Reeling:
		Reel();
		break;
	default:
		break;
	}
}

void UFishingBlueprintNode::SetFishState(const EFishState& InFishState)
{
	ActiveFishState = InFishState;
	if (UFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(PlayerActor))
	{
		FishingComponent->FishStateChanged(InFishState);
	}
}
