// Copyright Alien Shores


#include "AbilitySystem/Ability/Offensive/BeamGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "GameplayCueFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/TagUtils.h"

void UBeamGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this))
	{
		WaitInputRelease->OnRelease.AddDynamic(this, &UBeamGameplayAbility::OnInputRelease);
		ExecuteTask(WaitInputRelease);
	}
	if (UTargetDataUnderMouse* TargetDataUnderMouseTask = UTargetDataUnderMouse::CreateTargetDataUnderMouse(
		this
	))
	{
		TargetDataUnderMouseTask->HasMouseTarget.AddDynamic(this, &UBeamGameplayAbility::OnReceiveMouseData);
		TargetDataUnderMouseTask->HasNoTarget.AddDynamic(this, &UBeamGameplayAbility::OnRecieveNoTarget);
		ExecuteTask(TargetDataUnderMouseTask);
	}
}

void UBeamGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	SetMouseCursorVisible(true);
	SetMovementEnabled(true);
	EndAbilityOnTargets();
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(DelayTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
	}
	CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	PrimaryTarget = nullptr;
	CueActors.Empty();
	ActorGameplayCueParameters.Empty();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBeamGameplayAbility::SetMouseCursorVisible(const bool Visible) const
{
	if (CurrentActorInfo && CurrentActorInfo
	                        ->
	                        PlayerController.IsValid()
	)
	{
		CurrentActorInfo->PlayerController->SetShowMouseCursor(Visible);
	}
}

void UBeamGameplayAbility::SetMovementEnabled(const bool Enabled) const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* CharacterMovementComponent = Character
		                                                          ? Cast<UCharacterMovementComponent>(
			                                                          Character->GetMovementComponent()
		                                                          )
		                                                          : nullptr;
	if (CharacterMovementComponent)
	{
		if (!Enabled)
		{
			CharacterMovementComponent->DisableMovement();
		}
		else
		{
			CharacterMovementComponent->SetMovementMode(MOVE_Walking);
		}
	}
}

AActor* UBeamGameplayAbility::DetermineCueTarget(AActor* ActorHit) const
{
	if (IsValid(ActorHit) && ActorHit->Implements<UCombatInterface>() && !TagUtils::HasAnyTag(
		ActorHit,
		ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo())
	))
	{
		return ActorHit;
	}
	return GetAvatarActorFromActorInfo();
}

void UBeamGameplayAbility::DetermineCascadingTargets(AActor* CueTarget, TArray<AActor*>& OutCascadedTargets)
{
	const int32 CascadingTargetCount = GetCascadeTargetsCount();
	if (CascadingTargetCount <= 0)
	{
		return;
	}
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	if (IsValid(CueTarget))
	{
		ActorsToIgnore.AddUnique(CueTarget);
	}
	TArray<AActor*> ActorsInRange;
	UElectricCastleAbilitySystemLibrary::GetLiveActorsWithinRadius(
		GetAvatarActorFromActorInfo(),
		ActorsToIgnore,
		ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo()),
		HitLocation,
		GetCascadeRadius(),
		ActorsInRange
	);
	UElectricCastleAbilitySystemLibrary::GetClosestActors(
		CascadingTargetCount,
		HitLocation,
		ActorsInRange,
		OutCascadedTargets
	);
}

int32 UBeamGameplayAbility::GetCascadeTargetsCount() const
{
	if (bForceCascadeMax)
	{
		return MaxCascadeTargets;
	}
	return FMath::Min(GetAbilityLevel() - 1, MaxCascadeTargets);
}

void UBeamGameplayAbility::ApplyCueToTarget(AActor* CueTarget, const FGameplayCueParameters& Parameters)
{
	UGameplayCueFunctionLibrary::AddGameplayCueOnActor(
		CueTarget,
		LoopCueTag,
		Parameters
	);
	ActorGameplayCueParameters.Add(CueTarget, Parameters);
	CueActors.AddUnique(CueTarget);
}

void UBeamGameplayAbility::CascadeToActor(const AActor* FromActor, AActor* CascadeTarget)
{
	FGameplayCueParameters TargetCueParams = FGameplayCueParameters();
	TargetCueParams.TargetAttachComponent = FromActor->GetRootComponent();
	TargetCueParams.Location = CascadeTarget->GetActorLocation();
	TargetCueParams.SourceObject = CascadeTarget;
	ApplyCueToTarget(CascadeTarget, TargetCueParams);
}

void UBeamGameplayAbility::InitializeTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBeamGameplayAbility::OnTimerTick, DamageTick, true);
}

void UBeamGameplayAbility::ApplyDamage(AActor* DamageActor)
{
	if (!IsValid(DamageActor) || ICombatInterface::IsDead(DamageActor) || !HasAuthority(
		&CurrentActivationInfo
	))
	{
		return;
	}
	FMakeEffectSpecSignature ApplyDamageDelegate;
	ApplyDamageDelegate.BindLambda(
		[this](FGameplayEffectSpecHandle& Spec)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				Spec,
				DamageConfig.DamageTypeTag,
				GetDamageAtLevel(GetAbilityLevel())
			);
		}
	);
	UElectricCastleAbilitySystemLibrary::ApplyGameplayEffectSpec(
		CurrentActorInfo->AbilitySystemComponent.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DamageActor),
		DamageEffectClass,
		GetAbilityLevel(),
		&ApplyDamageDelegate
	);
}

void UBeamGameplayAbility::OnTimerTick()
{
	if (CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		if (!HasAuthority(&CurrentActivationInfo))
		{
			return;
		}
		TArray ApplyDamageActors(CueActors);
		ApplyDamageActors.RemoveAll(
			[this](AActor* Actor)
			{
				return Actor == GetAvatarActorFromActorInfo() || ICombatInterface::IsDead(Actor);
			}
		);

		for (AActor* Actor : ApplyDamageActors)
		{
			ApplyDamage(Actor);
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UBeamGameplayAbility::OnReceiveMouseData(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (const FHitResult HitResult = *DataHandle.Data[0].Get()->GetHitResult(); HitResult.bBlockingHit)
	{
		ExecuteAbility(HitResult);
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UBeamGameplayAbility::SpawnBeam(FGameplayEventData Payload)
{
	SetMovementEnabled(false);
	ICombatInterface::SetActiveAbilityTag(GetAvatarActorFromActorInfo(), GetDefaultAbilityTag());
	AActor* ActorHit = TraceFirstTarget(HitLocation);
	bool bIsEnemyHit = (IsValid(ActorHit) && ActorHit->Implements<UCombatInterface>());
	FGameplayCueParameters FirstTargetCueParams = FGameplayCueParameters();
	FirstTargetCueParams.TargetAttachComponent = ICombatInterface::GetWeapon(GetAvatarActorFromActorInfo());
	FirstTargetCueParams.Location = bIsEnemyHit
		                                ? ActorHit->GetActorLocation()
		                                : HitLocation;
	FirstTargetCueParams.SourceObject = ActorHit;
	if (bIsEnemyHit)
	{
		PrimaryTarget = ActorHit;
		BindPrimaryTargetDeath(ActorHit);
	}
	// if the hit actor is valid, implements the combat interface, and is an enemy target...
	if (AActor* CueTarget = DetermineCueTarget(ActorHit); IsValid(CueTarget))
	{
		ApplyCueToTarget(CueTarget, FirstTargetCueParams);
		if (bIsEnemyHit)
		{
			TArray<AActor*> CascadingTargets;
			DetermineCascadingTargets(CueTarget, CascadingTargets);
			for (AActor* CascadingTarget : CascadingTargets)
			{
				BindCascadeTargetDeath(CascadingTarget);
				CascadeToActor(CueTarget, CascadingTarget);
			}
		}
		InitializeTimer();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UBeamGameplayAbility::ExecuteAbility(const FHitResult& HitResult)
{
	HitLocation = HitResult.ImpactPoint;
	SetMouseCursorVisible(false);
	ICombatInterface::UpdateFacingTarget(GetAvatarActorFromActorInfo(), HitResult.ImpactPoint);
	FGameplayCueParameters CueParams = FGameplayCueParameters();
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(GetAvatarActorFromActorInfo(), SoundCueTag, CueParams);
	if (UAbilityTask_PlayMontageAndWait* PlayMontageAndWait =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AbilityMontage
		))
	{
		ExecuteTask(PlayMontageAndWait);
	}
	if (UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		EventTag,
		nullptr,
		true,
		true
	))
	{
		WaitGameplayEvent->EventReceived.AddDynamic(this, &UBeamGameplayAbility::SpawnBeam);
		ExecuteTask(WaitGameplayEvent);
	}
}

AActor* UBeamGameplayAbility::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	FHitResult HitResult;
	const FVector SocketLocation = ICombatInterface::GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	UKismetSystemLibrary::SphereTraceSingle(
		GetAvatarActorFromActorInfo(),
		SocketLocation,
		BeamTargetLocation,
		BeamTraceSize,
		UEngineTypes::ConvertToTraceType(ECC_Projectile),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	if (HitResult.bBlockingHit)
	{
		HitLocation = HitResult.ImpactPoint;
		return HitResult.GetActor();
	}
	return nullptr;
}

void UBeamGameplayAbility::OnDelayedRelease()
{
	EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBeamGameplayAbility::OnInputRelease(float TimeHeld)
{
	if (TimeHeld < MinimumSpellTime)
	{
		// Enforce minimum spell time
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimerHandle,
			this,
			&UBeamGameplayAbility::OnDelayedRelease,
			MinimumSpellTime - TimeHeld,
			false
		);
	}
	else
	{
		EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UBeamGameplayAbility::OnPrimaryTargetDead(AActor* TargetActor)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBeamGameplayAbility::OnCascadeTargetDead(AActor* CascadeTarget)
{
	if (ActorGameplayCueParameters.Contains(CascadeTarget))
	{
		UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(
			CascadeTarget,
			LoopCueTag,
			ActorGameplayCueParameters[CascadeTarget]
		);
	}
	ActorGameplayCueParameters.Remove(CascadeTarget);
	CueActors.Remove(CascadeTarget);
}

void UBeamGameplayAbility::BindPrimaryTargetDeath(AActor* Actor)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UBeamGameplayAbility::OnPrimaryTargetDead))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UBeamGameplayAbility::OnPrimaryTargetDead);
		}
	}
}

void UBeamGameplayAbility::BindCascadeTargetDeath(AActor* Actor)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UBeamGameplayAbility::OnCascadeTargetDead))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UBeamGameplayAbility::OnCascadeTargetDead);
		}
	}
}

bool UBeamGameplayAbility::IsTargetALivingEnemy(const AActor* TargetActor) const
{
	return ICombatInterface::IsAlive(TargetActor) && !TagUtils::HasAnyTag(
		TargetActor,
		ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo())
	);
}

void UBeamGameplayAbility::EndAbilityOnTargets()
{
	if (CueActors.Num() <= 0)
	{
		return;
	}
	for (TArray<AActor*> Targets(CueActors); AActor* CueActor : Targets)
	{
		if (CueActor && ActorGameplayCueParameters.Contains(CueActor))
		{
			UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(
				CueActor,
				LoopCueTag,
				ActorGameplayCueParameters[CueActor]
			);
		}
		if (IsTargetALivingEnemy(CueActor) && HasAuthority(&CurrentActivationInfo))
		{
			FDamageEffectParams LastHit = MakeDamageEffectParamsFromClassDefaults(CueActor);
			UElectricCastleAbilitySystemLibrary::ApplyDamageEffect(LastHit);
		}
	}
}

void UBeamGameplayAbility::OnRecieveNoTarget(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
