// Copyright Alien Shores


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/ElectricCastlePlayerController.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(
	UGameplayAbility* OwningAbility, bool bPreferLivingActors, float SweepRadius, float SweepDistance, bool bDebug
)
{
	UTargetDataUnderMouse* Task = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	Task->bPreferLivingActors = bPreferLivingActors;
	Task->SweepRadius = SweepRadius;
	Task->SweepDistance = SweepDistance;
	Task->bDebug = bDebug;
	return Task;
}

void UTargetDataUnderMouse::Activate()
{
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendMouseCursorDataToServer();
	}
	else
	{
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		FPredictionKey PredictionKey = GetActivationPredictionKey();
		// Add callback for when server receives target mouse data 
		AbilitySystemComponent
			.Get()
			->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey)
			.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		// invoke callback if it's already received the target mouse data
		if (!AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorDataToServer() const
{
	if (const AElectricCastlePlayerController* PlayerController = Cast<AElectricCastlePlayerController>(Ability->GetCurrentActorInfo()->PlayerController.Get()))
	{
		FHitResult CursorHit;
		FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
		FGameplayAbilityTargetDataHandle DataHandle;
		FVector TargetDirection = GetAvatarActor()->GetActorForwardVector();
		if (PlayerController->IsInputTypeMouse())
		{
			PlayerController->GetHitResultUnderCursor(ECC_Target, false, CursorHit);
			TargetDirection = (CursorHit.ImpactPoint - GetAvatarActor()->GetActorLocation()).GetSafeNormal();
			TargetDirection.Z = 0;
		}
		if (ShouldTrySphereTrace(CursorHit))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(GetAvatarActor());
			UKismetSystemLibrary::SphereTraceSingle(
				GetAvatarActor(),
				GetAvatarActor()->GetActorLocation(),
				GetAvatarActor()->GetActorLocation() + TargetDirection * SweepDistance,
				SweepRadius,
				UEngineTypes::ConvertToTraceType(ECC_Target),
				false,
				ActorsToIgnore,
				bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				CursorHit,
				true,
				FLinearColor::Green,
				FLinearColor::Red,
				5.f
			);
		}
		Data->HitResult = CursorHit;
		DataHandle.Add(Data);
		// create a prediction window for this ability system
		FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
		// replicate data to server
		AbilitySystemComponent->ServerSetReplicatedTargetData(
			GetAbilitySpecHandle(),
			GetActivationPredictionKey(),
			DataHandle,
			FGameplayTag(),
			AbilitySystemComponent->ScopedPredictionKey
		);
		// broadcast the ability locally if enabled
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			if (CursorHit.bBlockingHit)
			{
				HasMouseTarget.Broadcast(DataHandle);
			}
			else
			{
				HasNoTarget.Broadcast(DataHandle);
			}
		}
	}
}

bool UTargetDataUnderMouse::ShouldTrySphereTrace(const FHitResult& HitResult) const
{
	if (bPreferLivingActors && !ICombatInterface::IsAlive(HitResult.GetActor()))
	{
		return true;
	}
	return !HitResult.bBlockingHit;
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(
	const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag GameplayTag
) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		HasMouseTarget.Broadcast(DataHandle);
	}
}
